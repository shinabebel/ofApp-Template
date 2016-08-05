#version 450 core

uniform mat4 viewMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;

struct Particle
{
	vec3 position;
	float seed;
	vec3 velocity;
	float life;
	vec4 color;
};