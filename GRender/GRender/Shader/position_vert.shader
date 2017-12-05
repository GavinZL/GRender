#version 430
//顶点位置
in vec3 G_position;
//mvp矩阵
uniform mat4 G_mvpMatrix;
//统一着色颜色
uniform vec4 G_displayColor;
//中介颜色
out vec4 intermediateColor;

void main()
{
	gl_Position = G_mvpMatrix * vec4(G_position, 1.0);
	intermediateColor = G_displayColor;
}
