#version 430

in vec4 intermediateColor;

// ͳһ��ɫ��ɫ
uniform vec4 G_displayColor;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 rColor = intermediateColor.rgb * G_displayColor.a;
	vec3 aColor = G_displayColor.rgb * (1 - G_displayColor.a);

	fragColor = intermediateColor * G_displayColor;
}