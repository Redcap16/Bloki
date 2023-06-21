#type vertex
#version 330 compatibility

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aLightLevel;
layout (location = 3) in float aTransparency;
		
uniform mat4 mvpMatrix;

out vec2 TexCoord;		
out float lightLevel;		
out float transparency;		

void main()
{
	gl_Position = mvpMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//gl_Position = gl_ModelViewProjectionMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);

	TexCoord = aTexCoord;
	lightLevel = aLightLevel;
	transparency = aTransparency;
}

#type fragment
#version 330 core

in vec2 TexCoord;
in float lightLevel;
in float transparency;
out vec4 FragColor;

uniform sampler2D textureAtlas;

void main()
{
	FragColor = texture(textureAtlas, TexCoord);
	if (lightLevel > 0)
		FragColor = mix(FragColor, vec4(1, 1, 1, 1), lightLevel);
	FragColor.a = transparency;
}