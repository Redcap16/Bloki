#type vertex
#version 330 compatibility

layout (location = 1) in ivec3 aPos;
layout (location = 0) in vec2 aTexCoord;
layout (location = 2) in ivec3 aNormal;
layout (location = 3) in int aState;

//uniform mat4 modelMatrix;
uniform mat4 mvp;

out vec2 TexCoord;	
out vec3 Normal;
out float cosTheta;
flat out int State;	

uniform vec3 lightDir;
//uniform vec3 cameraPos;	

void main()
{
	gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0);

	TexCoord = aTexCoord;
	State = aState;
	Normal = aNormal;
	
	//angle = clamp(dot(normalize(cameraPos - vec3(modelMatrix * vec4(aPos, 1))), aNormal), 0, 1);
	//cosTheta = clamp(dot(normalize(lightDir), aNormal), 0, 0.8) + 0.2;
	
	//if(length(aNormal - vec3(0, -1, 0)) < 0.1)
	//	cosTheta = 0.2;
}

#type fragment
#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in float cosTheta;
flat in int State;
out vec4 FragColor;

uniform sampler2D textureAtlas;

void main()
{
	FragColor = texture(textureAtlas, TexCoord);
	if(FragColor.a < 0.1)
		discard;
		
	//FragColor = vec4(vec3(FragColor) * cosTheta, FragColor.a);
	
	if (State == 1)
		FragColor = mix(FragColor, vec4(1, 1, 1, 1), 0.1);
	//FragColor = vec4(1, 0, 0, 1);
	//FragColor = vec4(TexCoord, 0, 1);
}