#include <graphics/ShaderProgram.hpp>

	bool ShaderProgram::loadShader(std::string content, ShaderType type)
	{
		unsigned int shader;
		if (type == ShaderType::VERTEX)
			shader = glCreateShader(GL_VERTEX_SHADER);
		else if (type == ShaderType::FRAGMENT)
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
				((type == ShaderType::VERTEX) ? "vertex" : "fragment") +
				" shader cant compile; More info:\n" +
				infoLog;
			Debug::GetInstance().Log(message);
			return false;
		}

		if (type == ShaderType::VERTEX)
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
				st = ShaderType::VERTEX;
			else if (typeName == "fragment")
				st = ShaderType::FRAGMENT;
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
	//Uniform setters
	void ShaderProgram::SetUniform(std::string name, glm::mat4 value)
	{
		const int uniformLocation = getUniformLocation(name);
		if (uniformLocation == -1)
			return;

		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
	}
	void ShaderProgram::SetUniform(std::string name, glm::vec3 value)
	{
		const int uniformLocation = getUniformLocation(name);
		if (uniformLocation == -1)
			return;

		glUniform3fv(uniformLocation, 1, glm::value_ptr(value));
	}
	void ShaderProgram::SetUniform(std::string name, int value)
	{
		const int uniformLocation = getUniformLocation(name);
		if (uniformLocation == -1)
			return;

		glUniform1i(uniformLocation, value);
	}

	void ShaderProgram::SetUniform(std::string name, float value)
	{
		const int uniformLocation = getUniformLocation(name);
		if (uniformLocation == -1)
			return;

		glUniform1f(uniformLocation, value);
	}