#type vertex
#version 330 compatibility

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aLightLevel;
		
uniform mat4 mvpMatrix;

out vec2 TexCoord;		
out float lightLevel;		

void main()
{
	gl_Position = mvpMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);

	TexCoord = aTexCoord;
	lightLevel = aLightLevel;
}

#type fragment
#version 330 core

in vec2 TexCoord;
in float lightLevel;
out vec4 FragColor;

uniform sampler2D textureAlias;

void main()
{
	FragColor = texture(textureAlias, TexCoord);
	if (lightLevel > 0)
		FragColor = mix(FragColor, vec4(1, 1, 1, 1), lightLevel);
	FragColor.a = 1;
}