#type vertex
#version 330 core

layout (location = 0) in ivec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;
		
uniform mat4 mvp;

out vec2 TexCoord;		
out vec3 Color;		

void main()
{
	gl_Position = mvp * vec4(aPos.x, aPos.y, 0.0, 1.0);

	TexCoord = aTexCoord;
	Color = aColor;
}

#type fragment
#version 330 core

in vec2 TexCoord;
in vec3 Color;

out vec4 FragColor;

uniform sampler2D Texture;
uniform bool HasTexture;

void main()
{
	vec4 sampled = HasTexture ? texture(Texture, TexCoord) : vec4(1, 1, 1, 1);
	FragColor = vec4(Color, 1) * sampled;
}