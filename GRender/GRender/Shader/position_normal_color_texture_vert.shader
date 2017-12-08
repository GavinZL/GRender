#version 430

in vec3 G_position;
in vec3 G_normal;
in vec4 G_color;
in vec2 G_texture0Coord;

// 点光源位置
uniform vec3 G_pointLightSourcePosition[8];

// 聚光灯光源位置
uniform vec3 G_spotLightSourcePosition[8];

// 顶点到点光源方向
out vec3 inter_vertexToPointLightDirection[8];

// 顶点到聚光源方向
out vec3 inter_vertexToSpotLightDirection[8];

// mvp
uniform mat4 G_mvpMatrix;

// mv
uniform mat4 G_mvMatrix;

// normal matrix
uniform mat3 G_normalMatrix;

// out vertex
out vec3 inter_vertex;

// out normal
out vec3 inter_Normal;

// out texture coords
out vec2 inter_texCoord;

// out color
out vec4 inter_Color;

void main(void)
{
	// 转换到眼睛坐标
	vec4 ePosition = G_mvMatrix * vec4(G_position,1.0);

	for (int i = 0; i < 8; ++i)
	{
		inter_vertexToPointLightDirection[i] = G_pointLightSourcePosition[i].xyz - ePosition.xyz;
		inter_vertexToSpotLightDirection[i] = G_spotLightSourcePosition[i].xyz - ePosition.xyz;
	}

	inter_Normal = G_normalMatrix * G_normal;
	inter_vertex = ePosition.xyz;

	inter_texCoord = G_texture0Coord;
	inter_texCoord.y = 1.0 - inter_texCoord.y;

	inter_Color = G_color;
	gl_Position = G_mvpMatrix * vec4(G_position, 1.0);
}