#type vertex
#version 330 compatibility

layout (location = 0) in ivec3 aPos;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(1, 0, 0, 1);
}