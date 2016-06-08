#pragma include <header.glsl>

precision highp float;
precision highp int;

struct Particle {
	vec4 pos;
	vec4 ppos;
	vec4 home;
	vec4 color;
	vec4 properties;
	vec4 candidateLine;
	vec4 candidateTri;
};

layout(std140, binding = 0) buffer particle {
    Particle p[];
};

layout(points, invocations = 1) in; // points, lines, lines_adjacency, triangles, triangles_adjacency
layout(triangle_strip, max_vertices = 6) out; // points, line_strip, triangle_strip

in gl_PerVertex
{
	vec4 gl_Position;
} gl_in[];

in block
{
	Particle point;
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


void addTriangles(float v1, float v2, vec4 originPos)
{
	if (v1 < 0 || v2 < 0)
		return;
		
	gl_Position = originPos;
	Out.color = vec4(0.5, 0.5, 0.5, 0.5);
	EmitVertex();
	
	uint id;
	
	id = uint(v1);
	gl_Position = modelViewProjectionMatrix * p[id].pos;
	Out.color = vec4(0.6, 0.0, 1.0, 0.5);
	Out.color = p[id].color;
	EmitVertex();
	
	id = uint(v2);
	gl_Position = modelViewProjectionMatrix * p[id].pos;
	Out.color = vec4(0.0, 0.6, 1.0, 0.5);
	Out.color = p[id].color;
	EmitVertex();
	
	EndPrimitive();
}

void main()
{	
	for(int i = 0; i < gl_in.length(); ++i)
	{
		addTriangles(In[i].point.candidateLine.x, In[i].point.candidateLine.y, gl_in[i].gl_Position);
		addTriangles(In[i].point.candidateLine.z, In[i].point.candidateLine.w, gl_in[i].gl_Position);
	}
}

