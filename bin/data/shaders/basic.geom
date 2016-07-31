#pragma include <header.glsl>

layout(points, invocations = 1) in; // points, lines, lines_adjacency, triangles, triangles_adjacency
layout(triangle_strip, max_vertices = 4) out; // points, line_strip, triangle_strip

in gl_PerVertex
{
	vec4 gl_Position;
} gl_in[];

in block
{
	vec4 position;
} In[];

out gl_PerVertex
{
	vec4 gl_Position;
};

out block
{
	vec3 fragPos;
	vec4 color;
	vec3 normal;
	vec2 texCoord;
} Out;

void main()
{	
	for (int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = modelViewProjectionMatrix * gl_in[i].gl_Position;
		EmitVertex();
		
		gl_Position = modelViewProjectionMatrix * gl_in[i].gl_Position;
		EmitVertex();
		
		gl_Position = modelViewProjectionMatrix * gl_in[i].gl_Position;
		EmitVertex();
		
		gl_Position = modelViewProjectionMatrix * gl_in[i].gl_Position;
		EmitVertex();
		
		EndPrimitive();
	}
}

