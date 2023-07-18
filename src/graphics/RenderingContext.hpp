#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <graphics/ShaderProgram.hpp>
#include <graphics/Texture.hpp>
#include <graphics/Camera3D.hpp>

struct RenderingContext
{
public:
	Camera3D* CurrentCamera3D;
};