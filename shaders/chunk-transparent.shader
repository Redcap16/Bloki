#type vertex
#version 330 compatibility

layout (location = 1) in ivec3 aPos;
layout (location = 0) in vec2 aTexCoord;
layout (location = 2) in ivec3 aNormal;
		
uniform mat4 mvp;

out vec2 TexCoord;		

void main()
{
	gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	
	TexCoord = aTexCoord;
}

#type fragment
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureAtlas;

void main()
{
	FragColor = texture(textureAtlas, TexCoord);
}