#version 430

in vec4 intermediateColor;

// 统一着色颜色
uniform vec4 G_displayColor;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 rColor = intermediateColor.rgb * G_displayColor.a;
	vec3 aColor = G_displayColor.rgb * (1 - G_displayColor.a);

	fragColor = intermediateColor; //vec4(rColor + aColor, 1.0);
}