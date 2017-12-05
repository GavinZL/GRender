#version 430
//����λ��
in vec3 G_position;
//mvp����
uniform mat4 G_mvpMatrix;
//ͳһ��ɫ��ɫ
uniform vec4 G_displayColor;
//�н���ɫ
out vec4 intermediateColor;

void main()
{
	gl_Position = G_mvpMatrix * vec4(G_position, 1.0);
	intermediateColor = G_displayColor;
}
