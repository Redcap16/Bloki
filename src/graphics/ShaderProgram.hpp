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
#include <stdexcept>

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

	void uniformDoesntExist();
	bool loadShader(std::string content, ShaderType type);
	bool loadShaders(const std::string filename);
	bool setup();

	unsigned int getUniformLocation(std::string name);

public:
	ShaderProgram(std::string filename);

	void UseProgram();

	virtual void SetMVPMatrix(glm::mat4 matrix);

	inline unsigned int GetProgramID()
	{
		return programID;
	}

	//Uniform setters
	void SetUniform(std::string name, glm::mat4 value);
	void SetUniform(std::string name, glm::vec3 value);
	void SetUniform(std::string name, int value);
	void SetUniform(std::string name, float value);
};