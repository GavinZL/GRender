#version 430
//#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 G_position;
layout(location = 1) in vec3 G_normal;
layout(location = 2) in vec4 G_color;

uniform mat4 G_mvpMatrix;
uniform mat4 G_normalMatrix;
//uniform vec3 G_lightPosition;

out vec4 intermediateColor;
out vec3 intermediateNormal;
out vec3 intermediateVertex;
out vec3 intermediateLightPosition;
out vec3 intermediateEyePosition;

void main()
{
	intermediateColor = G_color;
	intermediateNormal = (G_normalMatrix * vec4(G_normal, 0.0)).xyz;
	intermediateVertex = (G_normalMatrix * vec4(G_position, 1.0)).xyz;
	intermediateLightPosition = (G_normalMatrix * vec4(10,10,10, 1.0)).xyz;
	intermediateEyePosition = -intermediateVertex;

	gl_Position = G_mvpMatrix * vec4(G_position, 1.0);

}
 