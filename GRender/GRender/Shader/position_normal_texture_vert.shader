#version 430
//#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 G_position;
layout(location = 1) in vec3 G_normal;
layout(location = 2) in vec2 G_texCoord0;

uniform mat4 G_mvpMatrix;
uniform mat4 G_mvMatrix;
uniform mat4 G_normalMatrix;
uniform vec4 G_lightPosition;

out vec2 texCoord0;
out vec3 mynormal;
out vec3 myvertex;
out vec3 mylightpos;
out vec3 myeyepos;

void main()
{
	gl_Position = G_mvpMatrix * vec4(G_position, 1.0);

	texCoord0 = G_texCoord0;
	mynormal = (G_normalMatrix * vec4(G_normal, 1.0)).xyz;
	myvertex = (G_mvMatrix * vec4(G_position, 1.0)).xyz;
	mylightpos = G_lightPosition.xyz;
	myeyepos  = -myvertex.xyz;
}
 