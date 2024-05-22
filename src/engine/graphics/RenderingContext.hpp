#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <engine/graphics/ShaderProgram.hpp>
#include <engine/graphics/Texture.hpp>
#include <engine/graphics/Camera3D.hpp>

struct RenderingContext
{
public:
	Camera3D* CurrentCamera3D;
};