#type vertex
#version 330 compatibility

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
//layout (location = 2) in float aLightLevel;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in int aState;

//uniform mat4 modelMatrix;
uniform mat4 mvpMatrix;

out vec2 TexCoord;	
out float cosTheta;
flat out int State;	

uniform vec3 lightDir;
//uniform vec3 cameraPos;	

void main()
{
	gl_Position = mvpMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);

	TexCoord = aTexCoord;
	State = aState;
	
	//angle = clamp(dot(normalize(cameraPos - vec3(modelMatrix * vec4(aPos, 1))), aNormal), 0, 1);
	cosTheta = clamp(dot(normalize(lightDir), aNormal), 0, 0.8) + 0.2;
	
	if(length(aNormal - vec3(0, -1, 0)) < 0.1)
		cosTheta = 0.2;
}

#type fragment
#version 330 core

in vec2 TexCoord;
in float cosTheta;
flat in int State;
out vec4 FragColor;

uniform sampler2D textureAtlas;

void main()
{
	FragColor = texture(textureAtlas, TexCoord);
	if(FragColor.a < 0.1)
		discard;
		
	FragColor = vec4(vec3(FragColor) * cosTheta, FragColor.a);
	
	if (State == 1)
		FragColor = mix(FragColor, vec4(1, 1, 1, 1), 0.1);
}