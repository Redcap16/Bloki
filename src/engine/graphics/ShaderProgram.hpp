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

#include <engine/util/Debug.hpp>
#include <engine/graphics/ErrorCheck.hpp>
#include <engine/core/Resource.hpp>
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
	ShaderProgram(ShaderProgram&& other) noexcept;
	ShaderProgram& operator=(ShaderProgram&& other) noexcept;

	void UseProgram() const;

	void SetMVPMatrix(const glm::mat4& matrix);

	inline ProgramHandle GetHandle() const { return m_ProgramHandle; }

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

template <>
struct ResourceParams<ShaderProgram>
{
	std::string Filename;

	ResourceParams(const std::string& filename) :
		Filename(filename) {}
};

inline bool operator==(const ResourceParams<ShaderProgram>& lhs, const ResourceParams<ShaderProgram>& rhs) noexcept
{
	return lhs.Filename == rhs.Filename;
}

template <>
class std::hash<ResourceParams<ShaderProgram>>
{
public:
	size_t operator()(const ResourceParams<ShaderProgram>& key) const
	{
		return std::hash<std::string>()(key.Filename);
	}
};