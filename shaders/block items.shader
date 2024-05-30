#type vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
		
uniform mat4 mvp;

out vec2 TexCoord;	
out vec3 Normal;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);

	TexCoord = aTexCoord;
	Normal = aNormal;
}

#type fragment
#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D Texture;
uniform vec2 TexturePosition,
	TextureSize;

void main()
{
	FragColor = texture(Texture, TexturePosition + TexCoord * TextureSize);
	//FragColor = vec4(TexturePosition + TexCoord * TextureSize, 0, 1);
}