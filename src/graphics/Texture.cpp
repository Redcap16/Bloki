#include <graphics/Texture.hpp>

static constexpr GLenum GetFormatFromChannelCount(unsigned short channelCount)
{
	switch (channelCount)
	{
	case 1:
		return GL_RED;
	case 2:
		return GL_RG;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	}

	return 0;
}