#version 330 core

uniform mat4 viewMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec4 globalColor;

in block {
	vec3 fragPos;
	vec4 color;
	vec3 normal;
	vec2 texCoord;
} In;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = In.color;
	//fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2));
}
