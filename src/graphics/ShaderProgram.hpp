#pragma once

#include <map>
#include <vector>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <util/Debug.hpp>
#include <graphics/ErrorCheck.hpp>
#include <stdexcept>

#define INVALID_UNIFORM_LOCATION -1

typedef GLint UniformLocation;
typedef GLuint ProgramHandle;
typedef GLuint ShaderHandle;

class ShaderProgram
{
public:
	enum class ShaderType
	{
		Vertex,
		Fragment
	};

	ShaderProgram(const std::string& filename);
	~ShaderProgram();
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& other);
	ShaderProgram& operator=(ShaderProgram&& other);

	void UseProgram() const;

	void SetMVPMatrix(const glm::mat4& matrix);

	inline ProgramHandle GetHandle() const;
	UniformLocation GetUniformLocation(const char* name) const;

	//Uniform setters
	void SetUniform(UniformLocation location, const glm::mat4& value);
	void SetUniform(UniformLocation location, const glm::vec3& value);
	void SetUniform(UniformLocation location, int value);
	void SetUniform(UniformLocation location, float value);

private:
	static constexpr const char* s_ShaderFolderPath = "shaders/";
	ProgramHandle m_ProgramHandle;

	UniformLocation m_MVPLocation;

	std::string m_Filename;

	ShaderHandle loadShader(const std::string& contents, ShaderType type);
	void unloadShader(ShaderHandle handle);
	bool loadShaders(ShaderHandle &vertex, ShaderHandle &fragment);

	void destroyProgram();
};