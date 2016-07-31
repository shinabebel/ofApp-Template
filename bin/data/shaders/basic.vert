#pragma include <header.glsl>

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 texCoord;

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
	Out.fragPos = (modelViewMatrix * position).xyz;
	Out.color = color;
	Out.texCoord = texCoord;
	Out.normal = mat3(transpose(inverse(modelViewMatrix))) * normal; 

	gl_Position = modelViewProjectionMatrix * position;
}