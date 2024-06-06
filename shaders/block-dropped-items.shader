#type vertex
#version 330 compatibility

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
		
uniform mat4 mvp;
uniform vec2 rot;

out vec2 TexCoord;		

void main() {
	vec3 pos = vec3(aPos.x * rot.x - aPos.z * rot.y, aPos.y, aPos.x * rot.y + aPos.z * rot.x);
	gl_Position = mvp * vec4(pos, 1.0);
	TexCoord = aTexCoord;
}

#type fragment
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D textureAtlas;
uniform vec2 TexturePosition,
	TextureSize;

void main() {
	FragColor = texture(textureAtlas, TexturePosition + TextureSize * TexCoord);
}