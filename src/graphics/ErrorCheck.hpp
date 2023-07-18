#pragma once

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <iostream>

bool CheckGLError(const char* file, unsigned int line);

#ifdef OGL_DEBUG
#define CHECK_GL_ERROR() CheckGLError(__FILE__, __LINE__)
#else
#define CHECK_GL_ERROR() false
#endif