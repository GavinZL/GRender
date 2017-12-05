#version 430

in vec3 G_position;
in vec4 G_color;

// mvpæÿ’Û
uniform mat4 G_mvpMatrix;

out vec4 intermediateColor;

void main()
{
	gl_Position = G_mvpMatrix * vec4(G_position, 1.0);
	intermediateColor = G_color;
}

