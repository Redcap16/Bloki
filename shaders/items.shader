#type vertex
#version 330 compatibility

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
		
uniform mat4 mvp;
uniform vec3 cameraRight,
	cameraUp;

out vec2 TexCoord;		

void main() {
	gl_Position = mvp * vec4(cameraRight * aPos.x + vec3(0, aPos.y, 0), 1.0);
	//gl_Position = mvp * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}

#type fragment
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureAtlas;

void main() {
	FragColor = texture(textureAtlas, TexCoord);
}