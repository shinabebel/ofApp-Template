#version 430 core

uniform mat4 viewMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;

layout (std140, binding = 0) uniform uniform_block {
    float time_step;
	float elapsed_time;
	float time_value;
	float a;
};