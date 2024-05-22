#include <engine/graphics/ErrorCheck.hpp>

bool CheckGLError(const char* file, unsigned int line)
{

    GLenum code = glGetError();
    if (code == GL_NO_ERROR)
        return false;

    char* message;
    while (code != GL_NO_ERROR)
    {
        switch (code)
        {
        case GL_INVALID_ENUM:
            message = (char*)"GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            message = (char*)"GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            message = (char*)"GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            message = (char*)"GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            message = (char*)"GL_OUT_OF_MEMORY";
            break;
        case GL_STACK_UNDERFLOW:
            message = (char*)"GL_STACK_UNDERFLOW";
            break;
        case GL_STACK_OVERFLOW:
            message = (char*)"GL_STACK_OVERFLOW";
        default:
            return true;
        }
        std::cout << "OpenGL " << message << " error ocurred at " << line << " in file " << file << std::endl;
        code = glGetError();
    }

    return true;
}