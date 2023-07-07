#include <graphics/ShaderProgram.hpp>

ShaderProgram::ShaderProgram(const std::string& filename) :
	m_ProgramHandle(0),
	m_MVPLocation(0),
	m_Filename(filename)
{
	m_ProgramHandle = glCreateProgram();
	if (CHECK_GL_ERROR() || !m_ProgramHandle)
	{
		DEBUG_LOG("Error while creating shader program at: " + filename);
		return;
	}

	ShaderHandle vertex, fragment;
	if (!loadShaders(vertex, fragment))
		return;

	glLinkProgram(m_ProgramHandle);

	GLint success;
	glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &success);

	GLint infoLogLength;
	glGetProgramiv(m_ProgramHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> infoLog(infoLogLength);
	glGetProgramInfoLog(m_ProgramHandle, infoLogLength, NULL, infoLog.data());

	if (!success)
	{
		DEBUG_LOG("Error: Cant link program " + filename + ". More info : \n" + infoLog.data());

		destroyProgram();
		return;
	}

	unloadShader(vertex);
	unloadShader(fragment);

	m_MVPLocation = GetUniformLocation("mvp");

	if (m_MVPLocation == INVALID_UNIFORM_LOCATION)
		DEBUG_LOG("Error: Cant find \"mvp\" location in vertex shader in " + filename + ".");
}

ShaderProgram::~ShaderProgram()
{
	destroyProgram();
}

ShaderProgram::ShaderProgram(ShaderProgram&& other)
{
	*this = std::move(other);
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other)
{
	if (this == &other)
	{
		destroyProgram();

		m_ProgramHandle = other.m_ProgramHandle;
		other.m_ProgramHandle = 0;

		m_Filename = other.m_Filename;
		m_MVPLocation = other.m_MVPLocation;
	}

	return *this;
}

void ShaderProgram::UseProgram() const
{
	glUseProgram(m_ProgramHandle);
	CHECK_GL_ERROR();
}

void ShaderProgram::SetMVPMatrix(const glm::mat4& matrix)
{
	SetUniform(m_MVPLocation, matrix);
}

ProgramHandle ShaderProgram::GetHandle() const
{
	return m_ProgramHandle;
}

UniformLocation ShaderProgram::GetUniformLocation(const char* name) const
{
	if (m_ProgramHandle == 0)
		return;

	const UniformLocation location = glGetUniformLocation(m_ProgramHandle, name);
	CHECK_GL_ERROR();

	if (location == INVALID_UNIFORM_LOCATION)
		DEBUG_LOG("Warnign: Can't find " + std::string(name) + " location in " + m_Filename);

	return location;
}

void ShaderProgram::SetUniform(UniformLocation location, const glm::mat4& value)
{
	if (m_ProgramHandle == 0)
		return;

	if (location == INVALID_UNIFORM_LOCATION)
		return;

	glUniform4fv(location, 1, glm::value_ptr(value));
	CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(UniformLocation location, const glm::vec3& value)
{
	if (m_ProgramHandle == 0)
		return;

	if (location == INVALID_UNIFORM_LOCATION)
		return;

	glUniform3fv(location, 1, glm::value_ptr(value));
	CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(UniformLocation location, int value)
{
	if (m_ProgramHandle == 0)
		return;

	if (location == INVALID_UNIFORM_LOCATION)
		return;

	glUniform1i(location, value);
	CHECK_GL_ERROR();
}

void ShaderProgram::SetUniform(UniformLocation location, float value)
{
	if (m_ProgramHandle == 0)
		return;

	if (location == INVALID_UNIFORM_LOCATION)
		return;

	glUniform1f(location, value);
	CHECK_GL_ERROR();
}

ShaderHandle ShaderProgram::loadShader(const std::string& contents, ShaderType type)
{
	ShaderHandle shader;
	if (type == ShaderType::Vertex)
		shader = glCreateShader(GL_VERTEX_SHADER);
	else if (type == ShaderType::Fragment)
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else
		return 0;

	const char* contentsp = contents.c_str();
	glShaderSource(shader, 1, &contentsp, NULL);

	glCompileShader(shader);

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	GLint logLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

	std::vector<char> infoLog(logLength);
	glGetShaderInfoLog(shader, logLength, NULL, infoLog.data());

	if (!success)
	{
		DEBUG_LOG("Error in " + m_Filename + ": " +
			((type == ShaderType::Vertex) ? "vertex" : "fragment") +
			" shader cant compile; More info: " +
			infoLog.data());

		glDeleteShader(shader);
		return 0;
	}
	
	if (logLength)
	{
		DEBUG_LOG("Info: Compilation of " + m_Filename + ": " +
			((type == ShaderType::Vertex) ? "vertex" : "fragment") +
			" shader successful. More info: " +
			infoLog.data());
	}
	else
	{
		DEBUG_LOG("Info: Compilation of " + m_Filename + ": " +
			((type == ShaderType::Vertex) ? "vertex" : "fragment") +
			" shader successful.");
	}

	glAttachShader(m_ProgramHandle, shader);

	CHECK_GL_ERROR();

	return shader;
}

void ShaderProgram::unloadShader(ShaderHandle shader)
{
	if (!shader)
		return;

	glDetachShader(m_ProgramHandle, shader);
	glDeleteShader(shader);

	CHECK_GL_ERROR();
}

bool ShaderProgram::loadShaders(ShaderHandle& vertex, ShaderHandle& fragment)
{
	const std::string filepath = s_ShaderFolderPath + m_Filename;
	std::fstream file = std::fstream(filepath, std::ios_base::in);

	if (!file.is_open())
		return false;

	std::string contents ( (std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()) );
	file.close();

	const char typeToken[] = "#type";
	std::map<ShaderType, std::string> sources;
	size_t index = contents.find(typeToken);

	while (index != std::string::npos)
	{
		index += strlen(typeToken);

		while (isspace(contents[index])) //Skip all whitespaces
		{
			if (index >= contents.size())
			{
				DEBUG_LOG("Error: Cant resolve shader type in " + m_Filename + ".");
				return false;
			}
			index++;
		}

		size_t nameBegin = index,
			nameLength = 0;
		while (!isspace(contents[nameBegin + nameLength])) //Save all letters
		{
			if (index >= contents.size())
			{
				DEBUG_LOG("Error: Cant resolve shader type in " + m_Filename + ".");
				return false;
			}
			nameLength++;
		}

		std::string shaderTypeName = contents.substr(nameBegin, nameLength);

		ShaderType shaderType;
		if (shaderTypeName == "vertex")
			shaderType = ShaderType::Vertex;
		else if (shaderTypeName == "fragment")
			shaderType = ShaderType::Fragment;
		else
		{
			DEBUG_LOG("Error: Unknown shader type in " + m_Filename + ".");
			return false;
		}

		if (sources.find(shaderType) != sources.end())
		{
			DEBUG_LOG("Error: Two shaders of the same type (" + shaderTypeName + ").");
			return false;
		}

		contents = contents.substr(nameBegin + nameLength);

		index = contents.find(typeToken);

		sources[shaderType] = contents.substr(0, index);
	}

	std::map<ShaderType, std::string>::iterator source = sources.find(ShaderType::Vertex);
	if (source == sources.end())
	{
		DEBUG_LOG("Error: Cant find vertex shader in " + m_Filename + ".");
		return false;
	}
	vertex = loadShader(source->second, ShaderType::Vertex);

	source = sources.find(ShaderType::Fragment);
	if (source == sources.end())
	{
		DEBUG_LOG("Error: Cant find fragment shader in " + m_Filename + ".");
		return false;
	}
	fragment = loadShader(source->second, ShaderType::Fragment);
	
	return true;
}

void ShaderProgram::destroyProgram()
{
	if (m_ProgramHandle)
	{
		glDeleteProgram(m_ProgramHandle);
		CHECK_GL_ERROR();

		m_ProgramHandle = 0;
	}
}

bool ShaderProgram::loadShader(std::string content, ShaderType type)
{
	unsigned int shader;
	if (type == ShaderType::Vertex)
		shader = glCreateShader(GL_VERTEX_SHADER);
	else if (type == ShaderType::Fragment)
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else
		return false;

	const char* contentsp = content.c_str();
	glShaderSource(shader, 1, &contentsp, NULL);

	glCompileShader(shader);

	int  success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		const std::string message = std::string("Error: ") +
			((type == ShaderType::Vertex) ? "vertex" : "fragment") +
			" shader cant compile; More info:\n" +
			infoLog;
		Debug::GetInstance().Log(message);
		return false;
	}

	if (type == ShaderType::Vertex)
		vertexShader = shader;
	else
		fragmentShader = shader;

	return true;
}

bool ShaderProgram::loadShaders(const std::string filename)
{
	const std::string filepath = shaderPath + filename;
	std::fstream file = std::fstream(filepath, std::ios_base::in);
	if (!file.is_open())
		return false;

	std::string contents;
	char buffer[256 + 1];
	while (file.good())
	{
		memset(buffer, 0, 256 + 1);
		file.read(buffer, 256);
		contents.append(buffer);
	}
	file.close();

	const char typeToken[] = "#type";

	std::string typeName;
	ShaderType st;
	std::vector<std::pair<std::string, ShaderType>> sources;
	size_t index = contents.find(typeToken),
		begin;

	if (index == std::string::npos)
	{
		Debug::GetInstance().Log("Error: cant resolve shader type.\n");
		return false;
	}

	while (true)
	{
		typeName.clear();
		index += strlen(typeToken);
		while (true) //Skip all whitespaces
		{
			if (index >= contents.size())
			{
				Debug::GetInstance().Log("Error: cant resolve shader type.\n");
				return false;
			}
			if (!isspace(contents[index]))
				break;
			index++;
		}
		while (true) //Save all letters
		{
			if (index >= contents.size())
			{
				Debug::GetInstance().Log("Error: cant resolve shader type.\n");
				return false;
			}
			if (isspace(contents[index]))
				break;

			typeName.push_back(tolower(contents[index]));
			index++;
		}
		begin = index;

		if (typeName == "vertex")
			st = ShaderType::Vertex;
		else if (typeName == "fragment")
			st = ShaderType::Fragment;
		else
		{
			Debug::GetInstance().Log("Error: cant resolve shader type.\n");
			return false;
		}

		int newIndex = contents.substr(index).find(typeToken);
		if (newIndex == std::string::npos)
			break;
		index += newIndex;


		sources.push_back({ contents.substr(begin, index - begin), st });
	}

	sources.push_back({ contents.substr(begin), st });

	for (auto it = sources.begin(); it != sources.end(); it++)
		if (!loadShader(it->first, it->second))
		{
			Debug::GetInstance().Log("Error loading shader.\n");
			return false;
		}

	return true;
}

void ShaderProgram::uniformDoesntExist()
{
	throw std::invalid_argument("Uniform variable with given name doesn't exist");
}

bool ShaderProgram::setup()
{
	if (programID)
		return false;
	if (!vertexShader || !fragmentShader)
		return false;

	programID = glCreateProgram();

	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	int  success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		Debug::GetInstance().Log(std::string("Error: cant link program. More info: \n") + infoLog);

		return false;
	}
	glUseProgram(programID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(0);

	return true;
}
unsigned int ShaderProgram::getUniformLocation(std::string name)
{
	if (!programID)
		return -1;
	if (uniformLocations.find(name) != uniformLocations.end())
		return uniformLocations[name];

	const int uniformLoc = glGetUniformLocation(programID, name.c_str());
	if (uniformLoc == -1)
		return -1;
	else
	{
		uniformLocations[name] = uniformLoc;
		return uniformLoc;
	}
}

ShaderProgram::ShaderProgram(std::string filename) :
	uniformLocations()
{
	if (!loadShaders(filename))
	{
		Debug::GetInstance().Log("Error loading shaders.\n");
		return;
	}
	if (!setup())
	{
		Debug::GetInstance().Log("Error initializing program.\n");
		return;
	}
}

void ShaderProgram::UseProgram()
{
	if (!programID)
		return;
	glUseProgram(programID);
}

void ShaderProgram::SetMVPMatrix(glm::mat4 matrix)
{
	SetUniform("mvpMatrix", matrix);
}

//Uniform setters
void ShaderProgram::SetUniform(std::string name, glm::mat4 value)
{
	const int uniformLocation = getUniformLocation(name);
	if (uniformLocation == -1)
		uniformDoesntExist();

	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}
void ShaderProgram::SetUniform(std::string name, glm::vec3 value)
{
	const int uniformLocation = getUniformLocation(name);
	if (uniformLocation == -1)
		uniformDoesntExist();

	glUniform3fv(uniformLocation, 1, glm::value_ptr(value));
}
void ShaderProgram::SetUniform(std::string name, int value)
{
	const int uniformLocation = getUniformLocation(name);
	if (uniformLocation == -1)
		uniformDoesntExist();

	glUniform1i(uniformLocation, value);
}

void ShaderProgram::SetUniform(std::string name, float value)
{
	const int uniformLocation = getUniformLocation(name);
	if (uniformLocation == -1)
		uniformDoesntExist();

	glUniform1f(uniformLocation, value);
}