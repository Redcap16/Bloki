#type vertex
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
		
uniform mat4 mvpMatrix;

out vec2 TexCoord;		

void main()
{
	gl_Position = mvpMatrix * vec4(aPos.x, aPos.y, 0.0, 1.0);

	TexCoord = aTexCoord;
}

#type fragment
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureAlias;
uniform vec3 textColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textureAlias, TexCoord).r);
	FragColor = vec4(textColor, 1.0) * sampled;
}