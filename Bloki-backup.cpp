#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <iostream>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <windowsx.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <window.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <stb_image.h>
#include <vector>
#include <db_perlin.h>
#include <map>
#include <fstream>
#include <ft2build.h>
#include <sstream>
#include <queue>
#include <cctype>
#include <ctime>
#include <unordered_map>
#include FT_FREETYPE_H  

#define BETWEEN(x, min, max) (x >= min && x <= max)
#define TORAD(x) ((x)*M_PI/180.0f)
#define TODEG(x) ((x)*180/M_PI);

/*
	 ________  ___  ___        ___   _______          _______
	/  _____/ /  / /  /       /  /  /  ___  \        /  ___  \
   /  /___   /  / /  /       /  /  /  /__/  /       /  /__/  /
  /  ____/  /  / /  /       /  /  /  ______/       /  ______/
 /  /      /  / /  /_____  /  /  /  /             /  /    __
/__/      /__/ /________/ /__/  /__/             /__/    /_/
*/

bool running = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

inline bool IsBetween(float x, float min, float max)
{
	return x >= min && x <= max;
}

inline float ToRadian(float deg)
{
	return deg * M_PI / 180.0f;
}

inline float ToDegrees(float rad)
{
	return rad * 180.0f / M_PI;
}

class ShaderProgram
{
public:
	enum class ShaderType
	{
		VERTEX,
		FRAGMENT
	};
private:
	const std::string shaderPath = "shaders/";

	unsigned int programID = 0,
		vertexShader = 0,
		fragmentShader = 0;
	std::map<std::string, unsigned int> uniformLocations;

	bool loadShader (std::string content, ShaderType type)
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
			std::cout << "Error: " << ((type == ShaderType::VERTEX) ? "vertex" : "fragment") << " shader cant compile; More info:\n" << infoLog << std::endl;
			return false;
		}  
		
		if (type == ShaderType::VERTEX)
			vertexShader = shader;
		else
			fragmentShader = shader;

		return true;
	}

	bool loadShaders(const std::string filename)
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
		if(index == std::string::npos)
		{
			std::cout << "Error: cant resolve shader type.\n";
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
					std::cout << "Error: cant resolve shader type.\n";
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
					std::cout << "Error: cant resolve shader type.\n";
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
				std::cout << "Error: cant resolve shader type.\n";
				return false;
			}

			int newIndex = contents.substr(index).find(typeToken);
			if (newIndex == std::string::npos)
				break;
			index += newIndex;


			sources.push_back({ contents.substr(begin, index - begin), st });
		}

		sources.push_back({ contents.substr(begin), st });

		for(auto it = sources.begin(); it != sources.end(); it++)
			if (!loadShader(it->first, it->second))
			{
				std::cout << "Error loading shader.\n";
				return false;
			}

		return true;
	}

	bool setup()
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
			std::cout << "Error: cant link program. More info: \n" << infoLog << std::endl;
			
			return false;
		}
		glUseProgram(programID);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glUseProgram(0);

		return true;
	}
	unsigned int getUniformLocation(std::string name)
	{
		if (!programID)
			return 0;
		if (uniformLocations.find(name) != uniformLocations.end())
			return uniformLocations[name];
		
		const unsigned int uniformLoc = glGetUniformLocation(programID, name.c_str());
		if (uniformLoc == -1)
			return -1;
		else
		{
			uniformLocations[name] = uniformLoc;
			return uniformLoc;
		}
	}
public:
	ShaderProgram(std::string filename) :
		uniformLocations()
	{
		if (!loadShaders(filename))
		{
			std::cout << "Error loading shaders.\n";
			return;
		}
		if (!setup())
		{
			std::cout << "Error initializing program.\n";
			return;
		}
	}

	inline unsigned int GetProgramID()
	{
		return programID;
	}
	void UseProgram()
	{
		if (!programID)
			return;
		glUseProgram(programID);
	}
	//Uniform setters
	void SetUniform(std::string name, glm::mat4 value)
	{
		const unsigned int uniformLocation = getUniformLocation(name);
		if (uniformLocation == -1)
			return;

		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
	}
	void SetUniform(std::string name, glm::vec3 value)
	{
		const unsigned int uniformLocation = getUniformLocation(name);
		if (uniformLocation == -1)
			return;

		glUniform3fv(uniformLocation, 1, glm::value_ptr(value));
	}
	void SetUniform(std::string name, int value)
	{
		const unsigned int uniformLocation = getUniformLocation(name);
		if (uniformLocation == -1)
			return;

		glUniform1i(uniformLocation, value);
	}
};

struct Vertex2D
{
	float x, y;
	float u, v;
};

struct Vertex3D
{
	float x, y, z;
	float u, v;
	float light;
}; 

struct Vertex3DT : public Vertex3D
{
	float transparency;
};

enum class RenderPass
{
	Solid,
	Transparent,
};

struct VertexAttribute
{
	GLint size;
	GLenum type;
	VertexAttribute(GLint size, GLenum type) :
		size(size),
		type(type)
	{

	}
};

template <typename TVertex>
class VertexBuffer
{
private:
	GLuint vao = 0, vbo, ebo;
	unsigned int indicesCount;

	bool dynamic;

	std::vector<TVertex> vertices;
	std::vector<unsigned int> indices;

	void setupBuffers(std::vector<VertexAttribute> attributes)
	{
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		size_t stride = 0;
		for (unsigned int i = 0; i < attributes.size(); i++)
		{
			VertexAttribute& attrib = attributes[i];
		
			glVertexAttribPointer(i, attrib.size, attrib.type, GL_FALSE, sizeof(TVertex), (const void*)stride);
			glEnableVertexAttribArray(i);

			switch (attrib.type)
			{
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
				stride += 1 * attrib.size;
				break;
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
			case GL_HALF_FLOAT:
				stride += 2 * attrib.size;
				break;
			case GL_INT:
			case GL_UNSIGNED_INT:
			case GL_FIXED:
			case GL_FLOAT:
				stride += 4 * attrib.size;
				break;
			case GL_DOUBLE:
				stride += 8 * attrib.size;
				break;
			default:
				std::cout << "Error: Bad enum" << std::endl;
				return;
			}
		}

		glGenBuffers(1, &ebo);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void updateBuffers()
	{
		if (!vao)
			return;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 
			vertices.size() * sizeof(TVertex), 
			vertices.data(), 
			dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			indices.size() * sizeof(unsigned int), 
			indices.data(), 
			dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

		indicesCount = indices.size(); //In case of vector clear

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void clearBuffers()
	{
		if (!vao)
			return;

		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);

		glDeleteVertexArrays(1, &vao);

		vao = 0;
	}

	void clearVertices()
	{
		vertices.clear();
		indices.clear();

		std::vector<TVertex>().swap(vertices);
		std::vector<unsigned int>().swap(indices);
	}
public:
	VertexBuffer(std::vector<VertexAttribute> attributes, bool dynamic = false) :
		dynamic(dynamic)
	{
		setupBuffers(attributes);
	}

	~VertexBuffer()
	{
		clearBuffers();
		clearVertices();
	}

	inline void AddVertex(TVertex& vertex)
	{
		vertices.push_back(vertex);
	}

	void AddVertices(TVertex* vertex, unsigned int size)
	{
		if (vertex == nullptr)
			return;

		for (unsigned int i = 0; i < size; i++)
			vertices.push_back(vertex[i]);
	}

	inline unsigned int GetCurrentIndex()
	{
		return vertices.size();
	}

	inline void AddIndex(unsigned int index)
	{
		indices.push_back(index);
	}

	void AddIndices(unsigned int* index, unsigned int size)
	{
		if (index == nullptr)
			return;

		for (unsigned int i = 0; i < size; i++)
			indices.push_back(index[i]);
	}

	inline void UpdateBuffers()
	{		
		updateBuffers();
	}

	inline void ClearVertices()
	{
		clearVertices();
	}

	void Render()
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};

struct Cube
{

};

class Texture
{
private:
	static const short maxTextureIndex = 16;
protected:
	const std::string texturePath = "assets/textures/";
	unsigned int texture = 0;
	Texture()
	{
	}
public:
	void BindTexture(ShaderProgram& program, std::string name, unsigned short index)
	{
		if (index > maxTextureIndex)
			return;

		program.SetUniform(name, index);
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	unsigned int GetTextureID()
	{
		return texture;
	}
};

class Camera
{
public:
	virtual glm::mat4 GetCameraMatrix() = 0;
};

class Camera3D : public Camera
{
private:
	glm::vec3 position = glm::vec3(1.0);
	float horizontalRotation = 0,
		verticalRotation = 0;
	float aspect;
	const float fov = 45.0f,
		nearVal = 0.1f,
		farVal = 100.0f;

	glm::mat4 projectionMatrix,
		viewMatrix,
		cameraMatrix;

	void updateProjectionMatrix()
	{
		projectionMatrix = glm::perspective(fov, aspect, nearVal, farVal);
		cameraMatrix = projectionMatrix * viewMatrix;
	}

	void updateViewMatrix()
	{
		const glm::vec3 lookVec = GetDirection();
		const glm::vec3 rightVec = glm::normalize(glm::cross(glm::vec3(0, 1, 0), lookVec));
		const glm::vec3 upVec = glm::cross(lookVec, rightVec);

		viewMatrix = glm::lookAt(position, position + lookVec, upVec);
		cameraMatrix = projectionMatrix * viewMatrix;
	}
public:
	Camera3D(float aspect) :
		aspect(aspect)
	{
		updateProjectionMatrix();
		updateViewMatrix();
	}

	void SetAspectRatio(float aspect)
	{
		this->aspect = aspect;
		updateProjectionMatrix();
	}

	void Rotate(float horizontal, float vertical)
	{
		horizontalRotation += horizontal;
		verticalRotation += vertical;

		while (horizontalRotation > 360)
			horizontalRotation -= 360;
		while (horizontalRotation < 0)
			horizontalRotation += 360;

		while (verticalRotation > 90)
			verticalRotation = 90;
		while (verticalRotation < -90)
			verticalRotation = -90;
		updateViewMatrix();
	}

	void SetRotation(float horizontal, float vertical)
	{
		horizontalRotation = horizontal;
		verticalRotation = vertical;

		while (horizontalRotation > 360)
			horizontalRotation -= 360;
		while (horizontalRotation < 0)
			horizontalRotation += 360;

		while (verticalRotation > 90)
			verticalRotation = 90;
		while (verticalRotation < -90)
			verticalRotation = -90;
		updateViewMatrix();
	}

	inline void Move(glm::vec3 move)
	{
		position += move;
		updateViewMatrix();
	}

	inline void SetPosition(glm::vec3 position)
	{
		this->position = position;
		updateViewMatrix();
	}

	inline glm::vec2 GetRotation()
	{
		return { horizontalRotation, verticalRotation };
	}

	inline glm::vec3 GetPosition()
	{
		return position;
	}

	inline glm::mat4 GetCameraMatrix() override
	{
		return cameraMatrix;
	}

	glm::vec3 GetDirection()
	{
		/*const glm::mat4 inversed = glm::inverse(cameraMatrix);
		glm::vec4 worldPos = inversed * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		worldPos /= worldPos.w;*/
		return glm::normalize(glm::vec3(
			cos(ToRadian(verticalRotation)) * sin(ToRadian(horizontalRotation)),
			sin(ToRadian(verticalRotation)),
			-cos(ToRadian(verticalRotation)) * cos(ToRadian(horizontalRotation))
			));
	}
};

class Camera2D : public Camera
{
private:
	glm::mat4 cameraMatrix;
	glm::ivec2 windowSize;

	void calculateMatrix()
	{
		cameraMatrix = glm::ortho<glm::f32>(0, windowSize.x, windowSize.y, 0, -1, 1);//, 0);
	}
public:
	Camera2D(glm::ivec2 windowSize) :
		windowSize(windowSize)
	{
		calculateMatrix();
	}

	void SetWindowSize(glm::ivec2 windowSize)
	{
		this->windowSize = windowSize;
		calculateMatrix();
	}

	inline glm::mat4 GetCameraMatrix() override
	{
		return cameraMatrix;
	}
};

struct FontCharacter
{
	glm::vec2 uvLeftTop,
		uvSize;
	glm::ivec2 size,
		bearing;
	unsigned int advance;

	FontCharacter(glm::vec2 uvlt, glm::vec2 uvs, glm::ivec2 size, glm::ivec2 bearing, unsigned int adv) :
		uvLeftTop(uvlt),
		uvSize(uvs),
		size(size),
		bearing(bearing),
		advance(adv)
	{
	}
	
	FontCharacter() :
		FontCharacter(glm::vec2(0), glm::vec2(1), glm::ivec2(1), glm::ivec2(0), 0)
	{

	}
};

class Font : public Texture
{
private:
	const std::string fontPath = "assets/fonts/";
	std::map<char, FontCharacter> characters;

	bool setup(const std::string file, unsigned short size)
	{
		const std::string filePath = fontPath + file;

		FT_Library ft;
		FT_Face face;

		if (FT_Init_FreeType(&ft))
		{
			std::cout << "Error: Cant load FreeType library" << std::endl;
			return false;
		}

		if (FT_New_Face(ft, filePath.c_str(), 0, &face))
		{
			std::cout << "Error: Cant load font" << std::endl;
			return false;
		}

		FT_Set_Pixel_Sizes(face, 0, size);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		int max_dim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(256));
		int tex_width = 1;
		while (tex_width < max_dim) tex_width <<= 1;
		int tex_height = tex_width;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tex_width, tex_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glm::ivec2 actualPosition = glm::ivec2(1.0f);
		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "Error: Failed to load Glyph" << std::endl;
				continue;
			}

			if (actualPosition.x + face->glyph->bitmap.width > tex_width)
			{
				actualPosition.x = 0;
				actualPosition.y += (1 + (face->size->metrics.height >> 6));
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, actualPosition.x, actualPosition.y, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			const glm::ivec2 size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);

			FontCharacter character = {
				(glm::vec2)actualPosition / glm::vec2(tex_width, tex_height),
				(glm::vec2)size / glm::vec2(tex_width, tex_height),
				size,
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				(unsigned int)face->glyph->advance.x >> 6
			};
			characters[c] = character;

			actualPosition.x += face->glyph->bitmap.width + 1;
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}
public:
	Font(std::string file, unsigned short size)
	{
		if(!setup(file, size))
		{
			std::cout << "Error: cant load font: " << file << std::endl;
			return;
		}
	}

	FontCharacter& GetCharacter(char c)
	{
		return characters[c];
	}
};

class Element
{
private:
	glm::ivec2 position;
	void recalculateMatrix()
	{
		model = glm::translate(glm::mat4(1.0f), glm::vec3((glm::vec2)position, 0));
	}
protected:
	glm::mat4 model = glm::mat4(1.0f);
	inline glm::ivec2 GetPosition()
	{
		return position;
	}
public:
	Element (glm::ivec2 position = glm::ivec2(0)) :
		position(position)
	{
		if (position != glm::ivec2(0))
			recalculateMatrix();
	}
	void SetPosition(glm::ivec2 position)
	{
		this->position = position; 
		recalculateMatrix();
	}

	inline glm::mat4 GetModelMatrix()
	{
		return model;
	}

	virtual void Render() = 0;
}; 

class UIManager
{
private:
	ShaderProgram shader;
	std::vector <Element*> elements;

public:
	UIManager() :
		shader("ui.shader")
	{
	}

	static UIManager& GetInstance()
	{
		static UIManager instance;
		return instance;
	}

	void Render(Camera2D &camera)
	{
		shader.UseProgram();
		for (auto it = elements.begin(); it != elements.end(); it++)
		{
			const glm::mat4 mvp = camera.GetCameraMatrix() * (*it)->GetModelMatrix();
			UIManager::GetInstance().GetShaderProgram().SetUniform("mvpMatrix", mvp);

			(*it)->Render();
		}
	}

	void AddElement(Element* newElement)
	{
		if (!newElement)
			return;
		elements.push_back(newElement);
	}

	inline ShaderProgram& GetShaderProgram()
	{
		return shader;
	}
};

class Text : public Element
{
private:
	std::string text = "";
	unsigned int vao = 0,
		vbo = 0,
		ebo = 0,
		indicesCount = 0; 
	Font& font;

	glm::vec3 textColor = glm::vec3(0.0f);

	void setupBuffers()
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
	}

	void updateBuffers()
	{
		if (!vao)
			setupBuffers();

		std::vector<Vertex2D> vertices;
		std::vector<unsigned int> indices;

		int positionX = 0;
		for (unsigned int i = 0; i < text.length(); i++)
		{
			FontCharacter& character = font.GetCharacter(text[i]);
			Vertex2D newVertex;

			//Left top
			newVertex.x = positionX + character.bearing.x;
			newVertex.y = -character.bearing.y;
			newVertex.u = character.uvLeftTop.x;
			newVertex.v = character.uvLeftTop.y;
			vertices.push_back(newVertex);
			//Right top
			newVertex.x = positionX + character.bearing.x + character.size.x;
			newVertex.y = -character.bearing.y;
			newVertex.u = character.uvLeftTop.x + character.uvSize.x;
			newVertex.v = character.uvLeftTop.y;
			vertices.push_back(newVertex);
			//Right bottom
			newVertex.x = positionX + character.bearing.x + character.size.x;
			newVertex.y = -character.bearing.y + character.size.y;
			newVertex.u = character.uvLeftTop.x + character.uvSize.x;
			newVertex.v = character.uvLeftTop.y + character.uvSize.y;
			vertices.push_back(newVertex);
			//Left bottom
			newVertex.x = positionX + character.bearing.x;
			newVertex.y = -character.bearing.y + character.size.y;
			newVertex.u = character.uvLeftTop.x;
			newVertex.v = character.uvLeftTop.y + character.uvSize.y;
			vertices.push_back(newVertex);

			//First face LT RT RB    0 1 2
			//Second face LT RB LB   0 2 3
			indices.push_back(vertices.size() - 4 + 0);
			indices.push_back(vertices.size() - 4 + 1);
			indices.push_back(vertices.size() - 4 + 2);

			indices.push_back(vertices.size() - 4 + 0);
			indices.push_back(vertices.size() - 4 + 2);
			indices.push_back(vertices.size() - 4 + 3);

			positionX += (int)character.advance;
		}

		indicesCount = indices.size();

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_STATIC_DRAW); 
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1); 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
public:
	Text(Font& font, std::string text = "", glm::ivec2 position = glm::ivec2(0)) :
		Element(position),
		font(font),
		text(text)
	{
		if(text != "")
			updateBuffers();
	}
	void SetText(std::string text)
	{
		this->text = text;
		updateBuffers();
	}
	inline void SetColor(glm::vec3 color)
	{
		this->textColor = color;
	}
	void Render() override
	{
		if (!vao)
			return;

		font.BindTexture(UIManager::GetInstance().GetShaderProgram(), "textureAlias", 0);
		UIManager::GetInstance().GetShaderProgram().SetUniform("textColor", textColor);

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	}
};

class Debug
{
public:
private:
	std::vector<std::pair<std::string, std::string>> items;
	std::vector<Text*> texts;
	Font font;
	static const glm::vec3 color;
	static const glm::ivec2 initialPos;
	static const unsigned short fontSize = 20;

	Debug() :
		font("DejaVuSerif.ttf", fontSize)
	{
	}
public:
	
	~Debug()
	{
		for (auto it = texts.begin(); it != texts.end(); it++)
			delete (*it);
	}
	
	static Debug& GetInstance()
	{
		static Debug instance;
		return instance;
	}

	void AddDebugItem(std::string name)
	{
		items.push_back({ name, "" });

		const glm::ivec2 textPos = initialPos + glm::ivec2(0, fontSize * texts.size());
		texts.push_back(new Text(font, "", textPos));

		UIManager::GetInstance().AddElement(texts.back());
	}

	void ChangeDebugValue(std::string name, std::string value)
	{
		for (auto it = items.begin(); it != items.end(); it++)
			if ((*it).first == name)
			{
				(*it).second = value;
				texts[it - items.begin()]->SetText(name + ": " + value);
				break;
			}
	}
};
const glm::vec3 Debug::color = glm::vec3(0);
const glm::ivec2 Debug::initialPos = glm::ivec2(10, 30);

class ImageTexture : public Texture
{
private:
	bool load(const std::string path)
	{
		const std::string filePath = texturePath + path;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		return true;
	}
public:
	ImageTexture(const std::string path)
	{
		load(path);
	}

	~ImageTexture()
	{
		if (texture)
			glDeleteTextures(1, &texture);
	}
};

class AtlasTexture : public Texture
{
public:
	struct SubTexture
	{
		glm::vec2 uv;
		glm::vec2 size;

		SubTexture(glm::vec2 uv, glm::vec2 size) :
			uv(uv),
			size(size)
		{

		}

		SubTexture() :
			SubTexture(glm::vec2(0), glm::vec2(1.0f))
		{

		}
	};
private:
	glm::ivec2 textureSize;
	unsigned int textureCount,
		textureCounter = 0;

	void setup()
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureSize.x, textureSize.y * textureCount, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}
public:
	AtlasTexture(glm::ivec2 textureSize, unsigned int textureCount) : 
		textureSize(textureSize),
		textureCount(textureCount)
	{
		setup();
	}

	~AtlasTexture()
	{
		if(texture)
			glDeleteTextures(1, &texture);
	}

	std::pair<bool, SubTexture> Load(const std::string filename)
	{
		if(textureCounter == textureCount)
			return { false, SubTexture() };

		int width, height, nrChannels;
		
		const std::string filePath = texturePath + filename;

		unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
		if (!data || width != textureSize.x || height != textureSize.y)
		{
			std::cout << "Failed to load texture" << std::endl;
			return { false, SubTexture() };
		}

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, textureSize.y * textureCounter, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		
		stbi_image_free(data);

		textureCounter++;
		if(textureCounter == textureCount)
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0); 
		
		return { true, SubTexture(glm::vec2(0, (float)(textureCounter - 1) / textureCount), glm::vec2(1, 1.0f / textureCount)) };
	}
};

class NotInitializedEx : public std::logic_error
{
protected:
public:
	NotInitializedEx(const std::string message) :
		std::logic_error(message)
	{

	}
};

struct Block
{
	struct BlockData
	{
		std::string name,
			textureName;
		float transparency;
	}; 

	typedef unsigned short BlockType;

	BlockType type;

	static const unsigned int blockCount = 5;
	static const BlockData blockData[blockCount];

	class TextureManager
	{
	public:
		AtlasTexture texture;
	private:
		static const glm::ivec2 textureSize;

		std::vector<AtlasTexture::SubTexture> subTextures;

		void setupTextures()
		{
			for (BlockType i = 0; i < blockCount; i++)
			{
				if (blockData[i].textureName == "")
				{
					subTextures.push_back(AtlasTexture::SubTexture());
					continue;
				}

				const auto subTexture = texture.Load(blockData[i].textureName);
				if (!subTexture.first)
				{
					std::cout << "Cant load " << blockData[i].name << " texture." << std::endl;
					subTextures.push_back(AtlasTexture::SubTexture());
					continue;
				}

				subTextures.push_back(subTexture.second);
			}
		}

		TextureManager() :
			texture(textureSize, blockCount)
		{
			setupTextures();
		}
	public:

		static TextureManager& GetInstance()
		{
			static TextureManager instance;
			return instance;
		}

		AtlasTexture::SubTexture& GetSubTexture(BlockType id)
		{
			return subTextures[id];
		}
	};

	Block() :
		type(0)
	{
	}

	Block(BlockType type) :
		type(type)
	{
	}

	static AtlasTexture::SubTexture& GetSubTexture(BlockType type)
	{
		return TextureManager::GetInstance().GetSubTexture(type);
	}

	static BlockType GetIdByName(const std::string name) //Warning slow function
	{
		for (BlockType i = 0; i < blockCount; i++)
			if (name == blockData[i].name)
				return i;

		return 0;
	}
};

const glm::ivec2 Block::TextureManager::textureSize = glm::ivec2(64, 48);

const Block::BlockData Block::blockData[blockCount] = {
		{"Air", "", 1},
		{"Mud", "mud.png", 1},
		{"Grass", "grass.png", 1},
		{"Stone", "stone.png", 1},
		{"Water", "water.png", 0.7f} };

class Chunk;

class ChunkHandler
{
public:
	virtual Chunk* GetChunkByCoord(glm::ivec3 position) = 0;
};

class Chunk
{
public:
	static const int chunkSizeXZ = 16,
		chunkSizeY = 64;
protected:
	glm::ivec3 position;
	ChunkHandler &world;

	Block block[chunkSizeXZ][chunkSizeY][chunkSizeXZ];

	VertexBuffer<Vertex3D> vboSolid;
	VertexBuffer<Vertex3DT> vboTransparent;

	glm::ivec3 highlightedBlock = glm::ivec3(0);
	bool highlighted = false;
	const float highlightedLight = 0.3f;

	glm::ivec3 lastBlock = glm::ivec3(0);
	const unsigned int stepSize = 40960;

	bool toUpdate = false,
		loaded = false;

	inline bool blockVisible(Block::BlockType type, Block::BlockType neighbor)
	{
		static const Block::BlockType airID = Block::GetIdByName("Air"),
			waterID = Block::GetIdByName("Water");

		if (type == airID)
			return false;
		if (neighbor == airID)
			return true;

		if (type != waterID && neighbor != waterID)
			return false;
		if (type == waterID && neighbor == waterID)
			return false;
		if (type != waterID && neighbor == waterID)
			return true;
		if (type == waterID && neighbor != waterID)
			return false;

	}

	void pushFace(const unsigned int vertexIndices[4], 
		glm::ivec3 position, 
		Block::BlockType type,
		bool highlighted)
	{
		const float verticesc[5 * 14] = {
		   0, 0, 0, 0, 0.333333333f,
		   0, 1, 0, 0.25f, 0.333333f,
		   1, 1, 0, 0.25f, 0.66666f,
		   1, 0, 0, 0, 0.666666f,
		   0, 0, 1, 0.75f, 0.33333f,
		   0, 1, 1, 0.5f, 0.33333f,
		   1, 1, 1, 0.5f, 0.66666f,
		   1, 0, 1, 0.75, 0.66666f,
		   0, 0, 0, 1, 0.333333f,
		   1, 0, 0, 1, 0.666666f,
		   0, 0, 0, 0.25f, 0,
		   0, 0, 1, 0.5f, 0,
		   1, 0, 0, 0.25f, 1,
		   1, 0, 1, 0.5f, 1 };


		AtlasTexture::SubTexture& st = Block::GetSubTexture(type);

		static const Block::BlockType waterID = Block::GetIdByName("Water");

		if (type != waterID)
		{
			Vertex3D vertex;
			for (int i = 0; i < 4; i++)
			{
				vertex.x = (verticesc[5 * vertexIndices[i] + 0] + position.x);
				vertex.y = (verticesc[5 * vertexIndices[i] + 1] + position.y);
				vertex.z = (verticesc[5 * vertexIndices[i] + 2] + position.z);

				vertex.u = st.uv.x;
				vertex.v = st.uv.y;

				vertex.u += (verticesc[5 * vertexIndices[i] + 3] * st.size.x);
				vertex.v += (verticesc[5 * vertexIndices[i] + 4] * st.size.y);

				vertex.light = highlighted ? highlightedLight : 0.0f;
				vboSolid.AddVertex(vertex);
			}

			const unsigned int index = vboSolid.GetCurrentIndex();

			vboSolid.AddIndex(index - 4);
			vboSolid.AddIndex(index - 2);
			vboSolid.AddIndex(index - 3);

			vboSolid.AddIndex(index - 2);
			vboSolid.AddIndex(index - 1);
			vboSolid.AddIndex(index - 3);
		}
		else
		{
			Vertex3DT vertex;
			for (int i = 0; i < 4; i++)
			{
				vertex.x = (verticesc[5 * vertexIndices[i] + 0] + position.x);
				vertex.y = (verticesc[5 * vertexIndices[i] + 1] + position.y);
				vertex.z = (verticesc[5 * vertexIndices[i] + 2] + position.z);

				vertex.u = st.uv.x;
				vertex.v = st.uv.y;

				vertex.u += (verticesc[5 * vertexIndices[i] + 3] * st.size.x);
				vertex.v += (verticesc[5 * vertexIndices[i] + 4] * st.size.y);

				vertex.light = highlighted ? highlightedLight : 0.0f;

				vertex.transparency = Block::blockData[waterID].transparency;
				vboTransparent.AddVertex(vertex);
			}
			const unsigned int index = vboTransparent.GetCurrentIndex();

			vboTransparent.AddIndex(index - 4);
			vboTransparent.AddIndex(index - 2);
			vboTransparent.AddIndex(index - 3);

			vboTransparent.AddIndex(index - 2);
			vboTransparent.AddIndex(index - 1);
			vboTransparent.AddIndex(index - 3);
		}
	}

	void pushBlock(glm::ivec3 position, Block &current, Chunk* neighborhood[6])
	{
		if (!IsBetween(position.x, 0, chunkSizeXZ - 1) ||
			!IsBetween(position.y, 0, chunkSizeY - 1) ||
			!IsBetween(position.z, 0, chunkSizeXZ - 1))
			return;

		static const Block::BlockType airID = Block::GetIdByName("Air"),
			waterID = Block::GetIdByName("Water");
		const bool highlighted = this->highlighted && (position == highlightedBlock);

		if (current.type == airID)
			return;

		if ((position.x == chunkSizeXZ - 1 && (neighborhood[0] == nullptr ||
			blockVisible(block[position.x][position.y][position.z].type, neighborhood[0]->GetBlock(glm::ivec3(0, position.y, position.z)).first.type))) ||
			(position.x < chunkSizeXZ - 1 && 
				blockVisible(block[position.x][position.y][position.z].type, block[position.x + 1][position.y][position.z].type)))
		{
			const unsigned int vertexIndices[4] = { 2, 6, 12, 13 };

			pushFace(vertexIndices, position, current.type, highlighted);
		}
		if ((position.x == 0 && (neighborhood[1] == nullptr ||
			blockVisible(block[position.x][position.y][position.z].type, neighborhood[1]->GetBlock(glm::ivec3(chunkSizeXZ - 1, position.y, position.z)).first.type))) ||
			(position.x > 0 && 
				blockVisible(block[position.x][position.y][position.z].type, block[position.x - 1][position.y][position.z].type)))
		{
			const unsigned int vertexIndices[4] = { 5, 1, 11, 10 };

			pushFace(vertexIndices, position, current.type, highlighted);
		}
		if ((position.y == chunkSizeY - 1 && (neighborhood[2] == nullptr ||
			blockVisible(block[position.x][position.y][position.z].type, neighborhood[2]->GetBlock(glm::ivec3(position.x, 0, position.z)).first.type))) ||
			(position.y < chunkSizeY - 1 && 
				blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y + 1][position.z].type)))
		{
			const unsigned int vertexIndices[4] = { 5, 6, 1, 2 };

			pushFace(vertexIndices, position, current.type, highlighted);
		}
		if ((position.y == 0 && (neighborhood[3] == nullptr ||
			blockVisible(block[position.x][position.y][position.z].type, neighborhood[3]->GetBlock(glm::ivec3(position.x, chunkSizeY - 1, position.z)).first.type))) ||
			(position.y > 0 && 
				blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y - 1][position.z].type)))
		{
			const unsigned int vertexIndices[4] = { 4, 8, 7, 9 };

			pushFace(vertexIndices, position, current.type, highlighted);
		}
		if ((position.z == chunkSizeXZ - 1 && (neighborhood[4] == nullptr ||
			blockVisible(block[position.x][position.y][position.z].type, neighborhood[4]->GetBlock(glm::ivec3(position.x, position.y, 0)).first.type))) ||
			(position.z < chunkSizeXZ - 1 && 
				blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y][position.z + 1].type)))
		{
			const unsigned int vertexIndices[4] = { 5, 4, 6, 7 };

			pushFace(vertexIndices, position, current.type, highlighted);
		}
		if ((position.z == 0 && (neighborhood[5] == nullptr ||
			blockVisible(block[position.x][position.y][position.z].type, neighborhood[5]->GetBlock(glm::ivec3(position.x, position.y, chunkSizeXZ - 1)).first.type))) ||
			(position.z > 0 && 
				blockVisible(block[position.x][position.y][position.z].type, block[position.x][position.y][position.z - 1].type)))
		{
			const unsigned int vertexIndices[4] = { 0, 1, 3, 2 };

			pushFace(vertexIndices, position, current.type, highlighted);
		}
	}
	
public:
	Chunk(glm::ivec3 position, ChunkHandler &world) :
		vboSolid({ VertexAttribute(3, GL_FLOAT), 
			VertexAttribute(2, GL_FLOAT), 
			VertexAttribute(1, GL_FLOAT) }),
		vboTransparent({ VertexAttribute(3, GL_FLOAT), 
			VertexAttribute(2, GL_FLOAT), 
			VertexAttribute(1, GL_FLOAT), 
			VertexAttribute(1, GL_FLOAT) }),
		position(position),
		world(world)
	{

	}

	void CreateMesh()
	{
		toUpdate = false;

		vboSolid.ClearVertices();
		vboTransparent.ClearVertices();

		Chunk* neighborhood[6] = { world.GetChunkByCoord(position + glm::ivec3(1, 0, 0)),
		world.GetChunkByCoord(position + glm::ivec3(-1, 0, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, 1, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, -1, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, 0, 1)),
		world.GetChunkByCoord(position + glm::ivec3(0, 0, -1)) };

		for (int x = 0; x < chunkSizeXZ; x++)
			for (int y = 0; y < chunkSizeY; y++)
				for (int z = 0; z < chunkSizeXZ; z++)
				{
					const glm::ivec3 pos = { x, y, z };
					pushBlock(pos, block[x][y][z], neighborhood);
				}

		loaded = true;

		vboSolid.UpdateBuffers();
		vboTransparent.UpdateBuffers();
		vboSolid.ClearVertices();
		vboTransparent.ClearVertices();
	}
	
	bool CreateMeshPartially()
	{
		if (lastBlock == glm::ivec3(0))
		{
			vboSolid.ClearVertices();
			vboTransparent.ClearVertices();
		}

		int& x = lastBlock.x,
			& y = lastBlock.y,
			& z = lastBlock.z,
			counter = 0;

		Chunk* neighborhood[6] = { world.GetChunkByCoord(position + glm::ivec3(1, 0, 0)),
		world.GetChunkByCoord(position + glm::ivec3(-1, 0, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, 1, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, -1, 0)),
		world.GetChunkByCoord(position + glm::ivec3(0, 0, 1)),
		world.GetChunkByCoord(position + glm::ivec3(0, 0, -1)) };

		for (; x < chunkSizeXZ; x++)
		{
			for (; y < chunkSizeY; y++)
			{
				for (; z < chunkSizeXZ; z++)
				{
					const glm::ivec3 pos = { x, y, z };
					pushBlock(pos, block[x][y][z], neighborhood);

					counter++;
					if (counter > stepSize)
						break;
				}
				if (counter > stepSize)
					break;
				z = 0;
			}
			if (counter > stepSize)
				break;
			y = 0;
		}

		if (x == chunkSizeXZ) //Generating done
		{
			vboSolid.UpdateBuffers();
			vboTransparent.UpdateBuffers();
			vboSolid.ClearVertices();
			vboTransparent.ClearVertices();
			loaded = true;
			return true;
		}
		return false;
	}

	void Update(bool force = false)
	{
		if (force || toUpdate)
			CreateMesh();
	}

	void Render(RenderPass pass)
	{
		if (pass == RenderPass::Solid)
			vboSolid.Render();
		else if (pass == RenderPass::Transparent)
			vboTransparent.Render();
		else
			return;
	}

	std::pair<Block, bool> GetBlock(glm::uvec3 pos) //LOCAL POSITION
	{
		if (!BETWEEN(pos.x, 0, chunkSizeXZ) ||
			!BETWEEN(pos.y, 0, chunkSizeY) ||
			!BETWEEN(pos.z, 0, chunkSizeXZ))
			return { Block(), false };
	
		return { block[pos.x][pos.y][pos.z], true };
	}
	
	bool SetBlock(glm::uvec3 pos, Block &newBlock, bool update = true) //LOCAL POSITION
	{
		if (!BETWEEN(pos.x, 0, chunkSizeXZ) ||
			!BETWEEN(pos.y, 0, chunkSizeY) ||
			!BETWEEN(pos.z, 0, chunkSizeXZ))
			return false;

		block[pos.x][pos.y][pos.z] = newBlock;

		if(update)
			toUpdate = true;

		return true;
	}

	void SetHighlightedBlock(glm::ivec3 position)
	{
		highlighted = true;
		highlightedBlock = position; 

		toUpdate = true;
	}

	void DisableHighlightedBlock()
	{
		highlighted = false;
			
		toUpdate = true;
	}
	
	bool IsLoaded()
	{
		return loaded;
	}
};

class WorldGenerator
{
private:
	const unsigned int stepSize = 40960;

	struct GeneratedChunk
	{
		Chunk* chunk;
		glm::ivec3 position;
		glm::ivec3 lastBlock;
		bool blockGenerated = false;
	};

	std::deque<GeneratedChunk> generationQueue;

	WorldGenerator()
	{

	}

public:
	static WorldGenerator& GetInstance()
	{
		static WorldGenerator instance;
		return instance;
	}

	void Generate(Chunk* chunk, glm::ivec3 position)
	{
		GeneratedChunk gc;
		gc.chunk = chunk;
		gc.position = position;
		gc.lastBlock = glm::ivec3(0);
		generationQueue.push_back(gc);
		std::cout << "Adding chunk for generation at " << generationQueue.size() << std::endl;
	}

	bool RemoveFromGeneration(Chunk* chunk)
	{
		for (auto it = generationQueue.begin(); it != generationQueue.end(); it++)
		{
			if (it->chunk == chunk)
			{
				generationQueue.erase(it);
				return true;
			}
		}

		return false;
	}

	void Step()
	{
		if (generationQueue.empty())
			return;

		GeneratedChunk& chunk = generationQueue.front();

		static const Block::BlockType airID = Block::GetIdByName("Air"),
			grassID = Block::GetIdByName("Grass"),
			mudID = Block::GetIdByName("Mud"),
			stoneID = Block::GetIdByName("Stone"),
			waterID = Block::GetIdByName("Water");
			

		if (!chunk.blockGenerated) //Chunk in terrain generation step
		{
			if (chunk.position.y > 0) //only air
				chunk.blockGenerated = true;

			int& x = chunk.lastBlock.x,
				& y = chunk.lastBlock.y,
				& z = chunk.lastBlock.z,
				counter = 0;

			for (; x < Chunk::chunkSizeXZ; x++)
			{
				for (; z < Chunk::chunkSizeXZ; z++)
				{
					float perlinx = fmod((chunk.position.x * (float)Chunk::chunkSizeXZ + (float)x) / 60.0f, 256),
						perlinz = fmod((chunk.position.z * (float)Chunk::chunkSizeXZ + (float)z) / 60.0f, 256);
					if (perlinx < 0)
						perlinx += 256;
					if (perlinz < 0)
						perlinz += 256;
					int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;
					for (; y < Chunk::chunkSizeY; y++)
					{
						Block block = Block(airID);

						if (chunk.position.y < 0)
							block.type = stoneID;
						else if (chunk.position.y == 0)
						{
							if (y < 25 && y > height)
								block.type = waterID;
							else if (y == height)
								block.type = grassID;
							else if (y >= height - 3 && y < height)
								block.type = mudID;
							else if (y < height - 3)
								block.type = stoneID;
						}

						if (block.type != airID)
							chunk.chunk->SetBlock(glm::uvec3(x, y, z), block, false);

						counter++;
						if (counter > stepSize)
							break;
					}
					if (counter > stepSize)
						break;
					y = 0;
				}
				if (counter > stepSize)
					break;
				z = 0;
			}
			if (x == Chunk::chunkSizeXZ) //Done generating
				chunk.blockGenerated = true;
		}
		else
			if (chunk.chunk->CreateMeshPartially()) //done
			{
				generationQueue.pop_front();
				std::cout << "Done generating at " << generationQueue.size() << std::endl;
			}
	}

	bool IsDone()
	{
		return generationQueue.empty();
	}
};

class World : ChunkHandler
{
private:
	static const int chunkRadius = 8,
		chunkArrayX = chunkRadius * 2 + 1,
		chunkArrayY = 3,
		chunkArrayZ = chunkArrayX;
	Chunk* loadedChunks[chunkArrayX][chunkArrayY][chunkArrayZ] = { nullptr };
	glm::ivec3 centerChunk;

	glm::ivec3 highlightedBlock = glm::ivec3(0);
	bool highlighted = false;

	ShaderProgram shaderTransparent;
	ShaderProgram shaderSolid;

	void reloadChunks(glm::uvec3 newCenter)
	{
		Chunk* newChunks[chunkArrayX][chunkArrayY][chunkArrayZ];
		for (unsigned int x = 0; x < chunkArrayX; x++)
			for (unsigned int y = 0; y < chunkArrayY; y++)
				for (unsigned int z = 0; z < chunkArrayZ; z++)
				{
					const glm::ivec3 chunkPosition = {
						newCenter.x + x - (((int)chunkArrayX - 1) / 2),
						newCenter.y + y - (((int)chunkArrayY - 1) / 2),
						newCenter.z + z - (((int)chunkArrayZ - 1) / 2),
					};

					const glm::ivec3 chunkPositionInOldArray = chunkPosition - centerChunk;

					if (chunkPositionInOldArray.x <= (((int)chunkArrayX - 1) / 2) &&
						chunkPositionInOldArray.x >= -(((int)chunkArrayX - 1) / 2) &&
						chunkPositionInOldArray.y <= (((int)chunkArrayY - 1) / 2) &&
						chunkPositionInOldArray.y >= -(((int)chunkArrayY - 1) / 2) &&
						chunkPositionInOldArray.z <= (((int)chunkArrayZ - 1) / 2) &&
						chunkPositionInOldArray.z >= -(((int)chunkArrayZ - 1) / 2)) //It it in old array
					{
						Chunk*& loadedChunk = loadedChunks[chunkPositionInOldArray.x + (((int)chunkArrayX - 1) / 2)]
							[chunkPositionInOldArray.y + (((int)chunkArrayY - 1) / 2)]
							[chunkPositionInOldArray.z + (((int)chunkArrayZ - 1) / 2)];
						newChunks[x][y][z] = loadedChunk;
						loadedChunk = 0;
					}
					else //If not, load it
						newChunks[x][y][z] = loadChunk(chunkPosition);
				}

		for (unsigned int x = 0; x < chunkArrayX; x++)
			for (unsigned int y = 0; y < chunkArrayY; y++)
				for (unsigned int z = 0; z < chunkArrayZ; z++)
				{
					Chunk*& loadedChunk = loadedChunks[x][y][z];
					if (loadedChunk)
						unloadChunk(loadedChunk);

					loadedChunk = newChunks[x][y][z]; //Replace old by new
				}

		centerChunk = newCenter;
	}

	void loadChunks()
	{
		for (unsigned int x = 0; x < chunkArrayX; x++)
			for (unsigned int y = 0; y < chunkArrayY; y++)
				for (unsigned int z = 0; z < chunkArrayZ; z++)
				{
					const glm::ivec3 chunkPosition = glm::ivec3(x, y, z) + centerChunk - glm::ivec3(((chunkArrayX - 1) / 2), ((chunkArrayY - 1) / 2), ((chunkArrayZ - 1) / 2));
					if (loadedChunks[x][y][z])
						unloadChunk(loadedChunks[x][y][z]);
					loadedChunks[x][y][z] = loadChunk(chunkPosition);
				}
	}

	Chunk* loadChunk(glm::ivec3 position)
	{
		Chunk* chunk = nullptr;
		if (false)
		{
			//load chunk from file
		}
		else
			chunk = generateChunk(position);

		//chunk->CreateMesh();
		return chunk;
	}

	void unloadAll()
	{
		for (unsigned int x = 0; x < chunkArrayX; x++)
			for (unsigned int y = 0; y < chunkArrayY; y++)
				for (unsigned int z = 0; z < chunkArrayZ; z++)
					unloadChunk(loadedChunks[x][y][z]);
	}

	void unloadChunk(Chunk *chunk)
	{
		if (!chunk)
			return;
		
		if (!chunk->IsLoaded()) //Need to remove from loading queue
			WorldGenerator::GetInstance().RemoveFromGeneration(chunk);
		delete chunk;
	}
	
	Chunk* generateChunk(glm::ivec3 position)
	{
		Chunk* newChunk = new Chunk(position, *this);

		/*if (position.y > 0) //only air
			return newChunk;

		for (unsigned int x = 0; x < Chunk::chunkSizeXZ; x++)
			for (unsigned int z = 0; z < Chunk::chunkSizeXZ; z++)
			{
				float perlinx = fmod((position.x * (float)Chunk::chunkSizeXZ + (float)x) / 60.0f, 256),
					perlinz = fmod((position.z * (float)Chunk::chunkSizeXZ + (float)z) / 60.0f, 256);
				if (perlinx < 0)
					perlinx += 256;
				if (perlinz < 0)
					perlinz += 256;
				int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;
				for (unsigned int y = 0; y < Chunk::chunkSizeY; y++)
				{
					Block block = Block(Block::BlockType::AIR);

					if (position.y < 0)
						block.type = Block::BlockType::STONE;
					else if (position.y == 0)
					{
						if (y == height)
							block.type = Block::BlockType::GRASS;
						else if (y >= height - 3 && y < height)
							block.type = Block::BlockType::MUD;
						else if (y < height - 3)
							block.type = Block::BlockType::STONE;
					}

					if (block.type != Block::BlockType::AIR)
						newChunk->SetBlock(glm::uvec3(x, y, z), block);
				}
			}
		if (position.y > 0)
			return new Chunk();*/
		
		/*ChunkGenerating* newChunk = new ChunkGenerating();
		newChunk->position = position;
		chunkGenerated.push_back(newChunk);*/
		WorldGenerator::GetInstance().Generate(newChunk, position);

		return newChunk;
	}
public:
	World() :
		shaderSolid("chunk-solid.shader"),
		shaderTransparent("chunk-transparent.shader")
	{
		loadChunks();
	}

	~World()
	{
		unloadAll();
	}

	static inline World& GetInstance()
	{
		static World instance;
		return instance;
	}
	
	Chunk* GetChunk(glm::ivec3 position)  //GLOBAL POSITION
	{
		glm::vec3 chunkSize = { Chunk::chunkSizeXZ, Chunk::chunkSizeY, Chunk::chunkSizeXZ };
		position = glm::floor((glm::vec3)position / chunkSize);

		const glm::ivec3 chunkPositionInArray = position - centerChunk;

		if (chunkPositionInArray.x <= (((int)chunkArrayX - 1) / 2) &&
			chunkPositionInArray.x >= -(((int)chunkArrayX - 1) / 2) &&
			chunkPositionInArray.y <= (((int)chunkArrayY - 1) / 2) &&
			chunkPositionInArray.y >= -(((int)chunkArrayY - 1) / 2) &&
			chunkPositionInArray.z <= (((int)chunkArrayZ - 1) / 2) &&
			chunkPositionInArray.z >= -(((int)chunkArrayZ - 1) / 2)) //It it in array
		{
			return loadedChunks[chunkPositionInArray.x + (((int)chunkArrayX - 1) / 2)]
				[chunkPositionInArray.y + (((int)chunkArrayY - 1) / 2)]
				[chunkPositionInArray.z + (((int)chunkArrayZ - 1) / 2)];
		}

		return nullptr;
	}

	Chunk* GetChunkByCoord(glm::ivec3 position) override //By chunk position
	{
		const glm::ivec3 chunkPosition = position - centerChunk + glm::ivec3(((int)chunkArrayX - 1) / 2, 
			((int)chunkArrayY - 1) / 2, 
			((int)chunkArrayZ - 1) / 2);

		if (IsBetween(chunkPosition.x, 0, chunkArrayX - 1) &&
			IsBetween(chunkPosition.y, 0, chunkArrayY - 1) &&
			IsBetween(chunkPosition.z, 0, chunkArrayZ - 1))
			return loadedChunks[chunkPosition.x][chunkPosition.y][chunkPosition.z];
			
		return nullptr;
	}

	std::pair<Block, bool> GetBlock(glm::ivec3 position) //GLOBAL POSITION
	{
		Chunk* chunk = GetChunk(position);
		if(!chunk)
			return { Block(), false };

		return chunk->GetBlock(ToLocalPosition(position));
	}
	
	bool SetBlock(glm::ivec3 position, Block &block, bool update) //GLOBAL POSITION
	{
		Chunk* chunk = GetChunk(position);
		if (!chunk)
			return false;

		return chunk->SetBlock(ToLocalPosition(position), block, update);
	}

	void SetCenterChunk(glm::ivec3 position) //GLOBAL POSITION
	{
		const glm::ivec3 newCenterChunk = {
			position.x / (int)Chunk::chunkSizeXZ,
			position.y / (int)Chunk::chunkSizeY,
			position.z / (int)Chunk::chunkSizeXZ,
		};

		if (newCenterChunk != centerChunk)
			reloadChunks(newCenterChunk);
	}

	void Update()
	{
		WorldGenerator::GetInstance().Step();

		bool force = false;
		if (Window::GetInstance().GetKey('T'))
		{
			Window::GetInstance().SetKey('T', false);
			force = true;
		}

		for (unsigned int x = 0; x < chunkArrayX; x++)
			for (unsigned int y = 0; y < chunkArrayY; y++)
				for (unsigned int z = 0; z < chunkArrayZ; z++)
					loadedChunks[x][y][z]->Update(force);
	}
	
	void Render(Camera3D &camera, RenderPass pass)
	{
		ShaderProgram* current;
		if (pass == RenderPass::Solid) current = &shaderSolid;
		else if (pass == RenderPass::Transparent) current = &shaderTransparent;
		else return;
		
		current->UseProgram();

		Block::TextureManager::GetInstance().texture.BindTexture(*current, "textureAtlas", 0);

		for (int x = 0; x < chunkArrayX; x++)
		{
			for (int y = 0; y < chunkArrayY; y++)
			{
				for (int z = 0; z < chunkArrayZ; z++)
				{
					const glm::vec3 chunkPos = {
						(int)Chunk::chunkSizeXZ * (x - ((int)chunkArrayX / 2) + centerChunk.x),
						(int)Chunk::chunkSizeY * (y - ((int)chunkArrayY / 2) + centerChunk.y),
						(int)Chunk::chunkSizeXZ * (z - ((int)chunkArrayZ / 2) + centerChunk.z)
					};
					
					const glm::mat4 chunkTranslation = glm::translate(glm::mat4(1.0f), chunkPos),
						mvp = camera.GetCameraMatrix() * chunkTranslation;

					current->SetUniform("mvpMatrix", mvp);

					Chunk * const current = loadedChunks[x][y][z];
					current->Render(pass);
				}
			}
		}
	}

	glm::ivec3 ToLocalPosition(glm::ivec3 world)
	{
		return {
			world.x % Chunk::chunkSizeXZ >= 0 ? world.x % Chunk::chunkSizeXZ : (world.x % Chunk::chunkSizeXZ) + Chunk::chunkSizeXZ,
			world.y % Chunk::chunkSizeY >= 0 ? world.y % Chunk::chunkSizeY : (world.y % Chunk::chunkSizeY) + Chunk::chunkSizeY,
			world.z % Chunk::chunkSizeXZ >= 0 ? world.z % Chunk::chunkSizeXZ : (world.z % Chunk::chunkSizeXZ) + Chunk::chunkSizeXZ }; //To get modulo not remainder
	}

	glm::ivec3 ToWorldPosition(glm::ivec3 chunk, glm::ivec3 local)
	{
		return { chunk.x * Chunk::chunkSizeXZ + local.x,
			chunk.y * Chunk::chunkSizeY + local.y,
			chunk.z * Chunk::chunkSizeXZ + local.z };
	}

	void SetHighlightedBlock(glm::ivec3 position)
	{
		if (highlighted && highlightedBlock == position)
			return; //No need to change anything
		if (highlighted)
		{
			Chunk* const chunk = GetChunk(highlightedBlock);
			chunk->DisableHighlightedBlock();
		}
		else
			highlighted = true;

		highlightedBlock = position;

		Chunk* const chunk = GetChunk(highlightedBlock);
		chunk->SetHighlightedBlock(ToLocalPosition(highlightedBlock));
	}

	void DisableHighlightedBlock()
	{
		if (!highlighted)
			return; //Nothing to do
		Chunk* const chunk = GetChunk(highlightedBlock);
		chunk->DisableHighlightedBlock();
		highlighted = false;
	}

	std::pair<glm::ivec3, bool> GetHighlightedBlock()
	{
		return { highlightedBlock, highlighted };
	}
};

class Player
{
private:
	struct
	{
		bool looking = false;
		Block block;
		glm::ivec3 blockInFront; //Basically: position to place new blocks
		glm::ivec3 position = glm::ivec3(0);
	} lookatBlock;

	const float mouseSensitivity = 0.1f;
	const float speed = 0.05f;
	const float workingDistance = 3.0f;

	const float gravity = 0.1f;
	bool onGround = false;

	glm::vec3 velocity = glm::vec3(0);

	glm::vec3 position;
	glm::vec2 rotation;
	Camera3D *eyeCamera = nullptr;

	Player() :
		position(0, 50, 0),
		rotation(0)
	{
		Debug::GetInstance().AddDebugItem("coll");
		Debug::GetInstance().AddDebugItem("onGnd");
	}

	/*bool raycastCheck(glm::ivec3 position)
	{
		World& world = World::GetInstance();

		Chunk* chunk = world.GetChunk(position);
		Block block = world.GetBlock(position).first;

		if (block.type != Block::BlockType::AIR)
		{
			position.x = floor(position.x);
			position.y = floor(position.y);
			position.z = floor(position.z);
			if (!world.GetHighlightedBlock().second || world.GetHighlightedBlock().first != (glm::ivec3)position)
			{
				std::ostringstream oss;
				oss << position.x << " " << position.y << " " << position.z;
				Debug::GetInstance().ChangeDebugValue("hpos", oss.str());

				world.SetHighlightedBlock((glm::ivec3)position);
			}
			return true;
		}

		return false;
	}*/

	void raycast()
	{
		World& world = World::GetInstance();

		/*glm::vec3 delta = glm::vec3(0);
		delta.x = sin(ToRadian(rotation.x)) * cos(ToRadian(rotation.y));
		delta.z = -cos(ToRadian(rotation.x)) * cos(ToRadian(rotation.y));
		delta.y = sin(ToRadian(rotation.y));

		float step = std::fmax(std::abs(delta.x), std::fmax(std::abs(delta.y), std::abs(delta.z)));
		delta /= step;

		glm::vec3 cpos = position;

		bool hit = false;
		for (int i = 0; i < 1000; i++) //To not escape the world
		{
			points.push_back(glm::floor(cpos));
			if (raycastCheck(glm::floor(cpos)))
			{
				hit = true;
				break;
			}
			if (glm::floor(cpos + delta) - glm::floor(delta) != glm::floor(cpos)) //Move to the next spot and go back by "step"
			{
				points.push_back((glm::floor(cpos)) + glm::floor(delta));
				if (raycastCheck((glm::floor(cpos)) + glm::floor(delta)))
				{
					hit = true;
					break;
				}
			}

			if (i > 3 && Window::GetInstance().GetKey('G'))
			{
				Block newBlock = Block(Block::BlockType::STONE);
				world.SetBlock(glm::floor(cpos), newBlock, true);
				//if ((glm::ivec3)(cpos + delta) - glm::ivec3(glm::floor(delta)) != (glm::ivec3)cpos)
					//world.SetBlock(glm::floor(cpos) + glm::floor(glm::floor(delta)), newBlock, true);
			}
			cpos += delta;
		}
		if (!hit)
			world.DisableHighlightedBlock();*/
		
		static const Block::BlockType airID = Block::GetIdByName("Air");

		glm::vec3 step = eyeCamera->GetDirection(); 
		step = glm::normalize(step);
		const float stepSize = 0.03f;
		step *= stepSize;

		glm::vec3 cpos = position;
		glm::ivec3 oldIntegerCPos = glm::floor(cpos) - glm::vec3(1); //To make it different

		bool hit = false; 

		while(glm::distance(cpos, position) <= workingDistance)
		{
			const glm::ivec3 icpos = glm::floor(cpos);
			if (icpos != oldIntegerCPos)
			{
				Block block = world.GetBlock(icpos).first;

				if (block.type != airID)
				{
					world.SetHighlightedBlock(icpos);
					lookatBlock.looking = true;
					lookatBlock.position = icpos;
					lookatBlock.block = block;
					lookatBlock.blockInFront = oldIntegerCPos;
					hit = true;
					break;
				}

				oldIntegerCPos = icpos;
			}

			cpos += step;
		}
		if (!hit)
		{
			world.DisableHighlightedBlock();
			lookatBlock.looking = false;
		}
	}

	void destroyBlock()
	{
		static const Block::BlockType airID = Block::GetIdByName("Air");
		World& world = World::GetInstance();
		if (lookatBlock.looking)
		{
			Block b;
			b.type = airID;
			world.SetBlock(lookatBlock.position, b, true);
		}

		raycast(); //Update lookatBlock
	}

	void placeBlock()
	{
		static const Block::BlockType stoneID = Block::GetIdByName("Stone");
		
		World& world = World::GetInstance();
		if (lookatBlock.looking)
		{
			Block b;
			b.type = stoneID;
			world.SetBlock(lookatBlock.blockInFront, b, true);
		}

		raycast(); //Update lookatBlock
	}

	void mouseMoved(glm::ivec2 position)
	{
		Window& win = Window::GetInstance();
		const glm::ivec2 winSize = win.GetSize();
		const glm::ivec2 delta = position - (winSize / 2);
		rotation.x += delta.x * mouseSensitivity;
		rotation.y -= delta.y * mouseSensitivity;
		if (rotation.y > 90.0f)
			rotation.y = 90;
		else if (rotation.y < -90)
			rotation.y = -90;

		if (rotation.x < 0)
			rotation.x += 360;
		else if (rotation.x > 360)
			rotation.x -= 360;

		raycast();

		win.SetMousePos(winSize.x / 2, winSize.y / 2);
	}

	std::pair<bool, glm::ivec3> collide()
	{
		World &world = World::GetInstance();
		const float height = 1.8f;
		static const Block::BlockType airID = Block::GetIdByName("Air");

		for (float y = 0; y < ceil(height) + 1; y++)
		{
			const glm::ivec3 blockPosition = glm::floor(position + glm::vec3(0, -y, 0));
			const auto block = world.GetBlock(blockPosition);
			if (block.second && block.first.type != airID)
				return { true, blockPosition };
		}

		return { false, glm::ivec3(0) };
	}
	
	void checkIfOnGround()
	{
		if (World::GetInstance().GetBlock(glm::ivec3(glm::floor(position)) - glm::ivec3(0, -2, 0)).first.type != Block::GetIdByName("Air"))
			onGround = true;
		else
			onGround = false;
	}
public:
	static Player& GetInstance()
	{
		static Player player;
		return player;
	}

	void Update(float deltaTime)
	{
		Window& win = Window::GetInstance();

		if (!World::GetInstance().GetChunk(position)->IsLoaded())
			return;

		if (win.GetKey('W'))
		{
			position.x += sin(ToRadian(rotation.x)) * cos(ToRadian(rotation.y)) * speed;
			position.z += -cos(ToRadian(rotation.x)) * cos(ToRadian(rotation.y)) * speed;
			position.y += sin(ToRadian(rotation.y)) * speed;
			//glm::vec3 acc = eyeCamera->GetDirection();
			//acc.y = 0;
			//acc = glm::normalize(acc) * speed * deltaTime;
			//velocity += acc;
		}
		if (win.GetKey('A'))
		{
			position.x += sin(ToRadian((double)rotation.x - 90.0f)) * speed;
			position.z += -cos(ToRadian((double)rotation.x - 90.0f)) * speed;
		}
		if (win.GetKey('S'))
		{
			position.x += sin(ToRadian((double)rotation.x + 180.0f)) * cos(ToRadian(rotation.y)) * speed;
			position.z += -cos(ToRadian((double)rotation.x + 180.0f)) * cos(ToRadian(rotation.y)) * speed;
			position.y += -sin(ToRadian(rotation.y)) * speed;
		}
		if (win.GetKey('D'))
		{
			position.x += sin(ToRadian((double)rotation.x + 90.0f)) * speed;
			position.z += -cos(ToRadian((double)rotation.x + 90.0f)) * speed;
		}
		if (win.GetKey(VK_SHIFT))
		{
			position.y += -1 * speed;
		}
		if (win.GetKey(VK_SPACE))
		{
			position.y += 1 * speed;
		}
		Debug::GetInstance().ChangeDebugValue("hor", std::to_string(rotation.x));
		Debug::GetInstance().ChangeDebugValue("ver", std::to_string(rotation.y)); 

		/*checkIfOnGround();
		if (!onGround)
			velocity.y -= gravity * deltaTime;
		else
			velocity.y = 0;

		velocity /= 1 + (0.1f * deltaTime);
		position += velocity * deltaTime;*/

		World::GetInstance().SetCenterChunk(position);

		/*bool colides = collide().first;
		Debug::GetInstance().ChangeDebugValue("onGnd", onGround ? "true" : "false");
		Debug::GetInstance().ChangeDebugValue("coll", colides ? "true" : "false");

		if (colides)
		{
			position -= velocity;
			velocity = glm::vec3(0);
		}
		*/
		/*static glm::vec3 velocity2 = glm::vec3(0);
		velocity2.y -= gravity;
		position += velocity2;

		colides = collide().first;
		Debug::GetInstance().ChangeDebugValue("coll", colides ? "true" : "false");

		if (colides)
		{
			position -= velocity2;
			velocity2 = glm::vec3(0);
		}*/

		eyeCamera->SetPosition(position);
		eyeCamera->SetRotation(rotation.x, rotation.y);
	}

	void SetEyeCamera(Camera3D *camera)
	{
		eyeCamera = camera;
	}

	void OnEvent(Event& e)
	{
		Window& win = Window::GetInstance();

		switch (e.type)
		{
		case Event::EventType::MouseMoved:
			mouseMoved(e.params.mouse.position);
			break; 
		case Event::EventType::MouseClicked:
			if (e.params.mouseButton.button == Event::MouseButton::LMB)
				destroyBlock();
			else if (e.params.mouseButton.button == Event::MouseButton::RMB)
				placeBlock();
			break;
		}
	}

	inline Camera3D* GetEyeCamera()
	{
		return eyeCamera;
	}

	void Render(Camera3D &camera)
	{
		
	}
};

class Renderer
{
private:
	static Renderer* instance;

	glm::ivec2 windowSize;
	std::vector<Camera3D> cameras;
	Camera3D* currentCamera3d = nullptr;
	Camera2D camera2d;

	void initGL()
	{
		glShadeModel(GL_SMOOTH);
		glClearColor(0.0f, 1.0f, 1.0f, 0.0f);

		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glDepthFunc(GL_LEQUAL);

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	Renderer(glm::ivec2 windowSize) :
		windowSize(windowSize),
		camera2d(windowSize)
	{
		initGL();
	}
public:
	static Renderer& GetInstance()
	{
		if (!instance)
			throw NotInitializedEx("Renderer not initialized yet. Please initialize it before use.");
		return *instance;
	}

	static void Init(glm::ivec2 windowSize)
	{
		if (instance)
			return;
		instance = new Renderer(windowSize);
	}

	static void Close()
	{
		if (!instance)
			return;
		delete instance;
		instance = nullptr;
	}

	static inline bool IsInitialized()
	{
		return instance != nullptr;
	}

	void Resize(glm::ivec2 windowSize)
	{
		glViewport(0, 0, windowSize.x, windowSize.y);

		if (windowSize.y == 0)
			windowSize.y = 1;

		camera2d.SetWindowSize(windowSize);
		for (auto cam = cameras.begin(); cam != cameras.end(); cam++)
			(*cam).SetAspectRatio((float)windowSize.x / windowSize.y);
	}

	Camera3D* CreateCamera3D()
	{
		cameras.push_back(Camera3D((float)windowSize.x / windowSize.y));
		return &cameras.back();
	}

	void SetCamera3D(Camera3D* camera)
	{
		currentCamera3d = camera;
	}

	void RenderScene()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Draw 3D
		if (currentCamera3d)
		{
			glDepthMask(GL_TRUE);
			World::GetInstance().Render(*currentCamera3d, RenderPass::Solid);
			Player::GetInstance().Render(*currentCamera3d);

			glDepthMask(GL_FALSE);
			World::GetInstance().Render(*currentCamera3d, RenderPass::Transparent);
		}
		glDepthMask(GL_TRUE);

		//Draw 2D
		UIManager::GetInstance().Render(camera2d);
	}
};
Renderer* Renderer::instance = nullptr;

void event(Event& e)
{
	Window& win = Window::GetInstance();

	switch (e.type)
	{
	case Event::EventType::Resize:
		if(Renderer::IsInitialized())
			Renderer::GetInstance().Resize(e.params.window.size);
		break;
	}

	if(running)
		Player::GetInstance().OnEvent(e);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE   hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window& win = Window::GetInstance();
	bool fs = true;

	win.OpenConsole();

	if (MessageBox(NULL, L"Would You Like To Run In Fullscreen Mode?", L"Start FullScreen?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		fs = false;

	if (!win.Initialize(1280, 720, "Bloki Alpha 1", event, fs))
	{
		MessageBox(NULL, L"Initialization failed", L"Fail", MB_OK | MB_ICONERROR);
		return 0;
	}

	win.SetMousePos(640, 360);

	glewInit();

	Renderer::Init(glm::ivec2(1280, 720));
	Renderer& renderer = Renderer::GetInstance();

	Debug::GetInstance().AddDebugItem("FPS");

	Player::GetInstance().SetEyeCamera(renderer.CreateCamera3D());
	renderer.SetCamera3D(Player::GetInstance().GetEyeCamera());

	Debug::GetInstance().AddDebugItem("hor");
	Debug::GetInstance().AddDebugItem("ver");

	running = true;

	clock_t currentTime = clock(),
		lastTime = currentTime,
		frameCounter = 0;

	float deltaTime = 0;

	while (!win.Done())
	{
		win.CheckForEvents();
		if (win.Active())
		{
			/*glm::vec3 delta = glm::vec3(0);
				delta.x = sin(TORAD(hor)) * cos(TORAD(ver));
				delta.z = -cos(TORAD(hor)) * cos(TORAD(ver));
				delta.y = -sin(TORAD(ver));

				float step = std::fmax(std::abs(delta.x), std::fmax(std::abs(delta.y), std::abs(delta.z)));
				delta /= step;

				glm::vec3 cpos = position;

				printf("Delta: %f %f %f\n", delta.x, delta.y, delta.z);

				bool hit = false;
				for (int i = 0; i < 100; i++)
				{
					if (cpos.x > 80 || cpos.x < 0 ||
						cpos.y > 20 || cpos.y < 0 ||
						cpos.z > 80 || cpos.z < 0)
						break;
					const unsigned int chunkx = (int)cpos.x / 8,
						chunkz = (int)cpos.z / 8;
					short& block = world[chunkx][chunkz].block[(int)cpos.x - chunkx * 8][(int)cpos.y][(int)cpos.z - chunkz * 8];

					if (block)
					{
						cpos.x = floor(cpos.x);
						cpos.y = floor(cpos.y);
						cpos.z = floor(cpos.z);
						if (!highlighted || highPos != cpos)
						{
							highlighted = true;
							highPos = cpos;
						}
						//block = 2;
						//prepareChunk(world[chunkx][chunkz]);
						hit = true;
						break;
					}

					cpos += delta;
				}
				if (!hit)
					highlighted = false;
			}*/
			
			if (win.GetKey('P'))
			{
				win.SetKey('P', false);
				running = !running;
			}

			if (running)
			{
				World::GetInstance().Update();
				Player::GetInstance().Update(deltaTime / 1000.0f);
			}

			renderer.RenderScene();
			SwapBuffers(win.GetDeviceContext());
		}
		
		if (frameCounter >= 10)
		{
			lastTime = currentTime;
			currentTime = clock();
			deltaTime = currentTime - lastTime;
			const int fps = deltaTime != 0 ? ((float)frameCounter / (deltaTime / CLOCKS_PER_SEC)) : 0;
			char buffer[64];
			sprintf_s(buffer, "%i", fps);
			Debug::GetInstance().ChangeDebugValue("FPS", std::string(buffer));
			//std::cout << fps << std::endl;
			frameCounter = 0;
		}
		frameCounter++;
	}

	win.Close();
	return 0;
}