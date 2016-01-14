#version 430 core

uniform mat4 viewMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform float uElapseTime;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texCoord;

out gl_PerVertex {
	vec4 gl_Position;
};

out block {
	vec3 fragPos;
	vec4 color;
	vec3 normal;
	vec2 texCoord;
} Out;

void main()
{
	mat4 modelMatrix = inverse(viewMatrix) * modelViewMatrix;
	vec4 particlePosEye = modelMatrix * vec4(position, 1.0);
	
	Out.fragPos = particlePosEye.xyz;
	Out.color = color;
	Out.texCoord = texCoord;
	Out.normal = mat3(transpose(inverse(modelMatrix))) * normal; 

	gl_Position = projectionMatrix * viewMatrix * particlePosEye;
}