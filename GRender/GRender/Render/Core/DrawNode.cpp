
#include "GL/glew.h"
#include "DrawNode.h"

#include "Engine.h"
#include "GLProgram.h"
#include "GLProgramState.h"
#include "Renderer.h"

#include "../GLStateCache.h"
#include "../Comm/Utils.h"
USING_NAMESPACE_G


DrawNode::DrawNode()
: m_vbo(0)
, m_dirty(false)
, m_initialized(false)
, m_pointSize(1)
, m_lineWidth(1)
{

}

DrawNode::~DrawNode()
{
	glDeleteBuffers(1, &m_vbo);
}

bool DrawNode::init(Type _type, void* params)
{
	// 1..set glprogram
	this->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_COLOR));
	m_vertices.reserve(32);
	m_colors.reserve(32);

	switch (_type)
	{
	case G::DrawNode::POINT:
	case G::DrawNode::POINTS:
		m_type = GL_POINTS;
		break;
	case G::DrawNode::LINE_LOOP:
		m_type = GL_LINE_LOOP;
		break;
	case G::DrawNode::LINE:
	case G::DrawNode::AXIS:
	case G::DrawNode::BOX:
		m_type = GL_LINES;
		break;
	case G::DrawNode::CUBE:
	case G::DrawNode::SPHERE:
		m_type = GL_TRIANGLES;
		break;
	case G::DrawNode::SCANNER:
		break;
	case G::DrawNode::MAX:
		break;
	default:
		break;
	}
	return true;
}

void DrawNode::draw(Renderer* renderer, const Mat4& transform, unsigned int flags)
{
	// ##judged whether call drawxxx function
	if (m_initialized)
	{
		if (m_vertices.size() > 0){
			m_customCommand.init();
			m_customCommand.func = G_CALLBACK_0(DrawNode::onDraw, this, transform, flags);
			renderer->addCommand(&m_customCommand);
		}
	}
}

void DrawNode::onDraw(const Mat4& transform, unsigned int flags)
{
	auto glProgram = this->getGLProgram();
	glProgram->use();
	glProgram->setUniformsForBuiltins(transform);

	glProgram->setUniformLocationWith4f(glProgram->getUniformLocation(GLProgram::UNIFORM_NAME_DISPLAYCOLOR), 1.0f, 1.0f, 1.0f, 1.0f);

	glPointSize(m_pointSize);
	glLineWidth(m_lineWidth);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glDrawArrays(m_type, 0, m_vertices.size());

	glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_COLOR);
	glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_POSITION);

	glPointSize(1);
	glLineWidth(1);
}

void DrawNode::drawPoint(const Vec3& point, const float ptSize, const Color4& color)
{
	m_vertices.resize(1);
	m_colors.resize(1);
	m_vertices[0] = point;
	m_colors[0] = color;

	m_pointSize = ptSize;

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

	m_initialized = true;
}

void DrawNode::drawPoints(const std::vector<Vec3>& pts, const float ptSize, const Color4& color)
{
	m_vertices.clear();
	std::copy(pts.begin(), pts.end(), m_vertices.begin());
	m_colors.resize(pts.size());
	for (int i = 0, cnt = pts.size(); i < cnt; ++i){
		m_colors[i] = color;
	}
	
	m_pointSize = ptSize;

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

	m_initialized = true;
}

void DrawNode::drawLine(const Vec3& origin, const Vec3& destination, const float lineSize, const Color4& color)
{
	m_vertices.resize(2);
	m_colors.resize(2);
	m_vertices[0] = origin;
	m_vertices[1] = destination;

	m_colors[0] = color;
	m_colors[1] = color;

	m_lineWidth = lineSize;

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

	m_initialized = true;
}

void DrawNode::drawLineLoop(const std::vector<Vec3>& vertices, const float lineSize, const Color4& color)
{
	m_vertices.clear();
	std::copy(vertices.begin(), vertices.end(), m_vertices.begin());
	m_colors.resize(vertices.size());
	for (int i = 0, cnt = m_colors.size(); i < cnt; ++i){
		m_colors[i] = color;
	}

	m_lineWidth = lineSize;

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

	m_initialized = true;
}

void DrawNode::drawAxis(const float length, const float lineSize )
{	
	static std::vector<Vec3> v={
		Vec3(0, 0, 0) * length, Vec3(1, 0, 0) * length,
		Vec3(1, 0, 0) * length, Vec3(0.8, 0, 0.2) * length,
		Vec3(1, 0, 0) * length, Vec3(0.8, 0, -0.2) * length,

		Vec3(0, 0, 0) * length, Vec3(0, 1, 0) * length,
		Vec3(0, 1, 0) * length, Vec3(0.2, 0.8, 0) * length,
		Vec3(0, 1, 0) * length, Vec3(-0.2, 0.8, 0) * length,

		Vec3(0, 0, 0) * length, Vec3(0, 0, 1) * length,
		Vec3(0, 0, 1) * length, Vec3(0.2, 0, 0.8) * length,
		Vec3(0, 0, 1) * length, Vec3(-0.2, 0, 0.8) * length
	};

	m_vertices = v;

	static std::vector<Vec4> c = {
		Vec4(1.0, 0.0, 0.0, 1.0), Vec4(1.0, 0.0, 0.0, 1.0),
		Vec4(1.0, 0.0, 0.0, 1.0), Vec4(1.0, 0.0, 0.0, 1.0),
		Vec4(1.0, 0.0, 0.0, 1.0), Vec4(1.0, 0.0, 0.0, 1.0),

		Vec4(0.0, 1.0, 0.0, 1.0), Vec4(0.0, 1.0, 0.0, 1.0),
		Vec4(0.0, 1.0, 0.0, 1.0), Vec4(0.0, 1.0, 0.0, 1.0),
		Vec4(0.0, 1.0, 0.0, 1.0), Vec4(0.0, 1.0, 0.0, 1.0),

		Vec4(0.0, 0.0, 1.0, 1.0), Vec4(0.0, 0.0, 1.0, 1.0),
		Vec4(0.0, 0.0, 1.0, 1.0), Vec4(0.0, 0.0, 1.0, 1.0),
		Vec4(0.0, 0.0, 1.0, 1.0), Vec4(0.0, 0.0, 1.0, 1.0)
	};
	m_colors = c;

	m_lineWidth = lineSize;

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

	m_initialized = true;
}

void DrawNode::drawBox(const Vec3& min, const Vec3& max, const float lineSize)
{
	m_lineWidth = lineSize;

	m_vertices.resize(24);
	//
	m_vertices[0] = Vec3(min[0], min[1], min[2]);
	m_vertices[1] = Vec3(min[0], min[1], max[2]);

	m_vertices[2] = Vec3(min[0], min[1], min[2]);
	m_vertices[3] = Vec3(max[0], min[1], min[2]);

	m_vertices[4] = Vec3(min[0], min[1], min[2]);
	m_vertices[5] = Vec3(min[0], max[1], min[2]);

	//
	m_vertices[6] = Vec3(max[0], min[1], max[2]);
	m_vertices[7] = Vec3(min[0], min[1], max[2]);

	m_vertices[8] = Vec3(max[0], min[1], max[2]);
	m_vertices[9] = Vec3(max[0], max[1], max[2]);

	m_vertices[10] = Vec3(max[0], min[1], max[2]);
	m_vertices[11] = Vec3(max[0], min[1], min[2]);

	//
	m_vertices[12] = Vec3(min[0], max[1], max[2]);
	m_vertices[13] = Vec3(min[0], min[1], max[2]);

	m_vertices[14] = Vec3(min[0], max[1], max[2]);
	m_vertices[15] = Vec3(min[0], max[1], min[2]);

	m_vertices[16] = Vec3(min[0], max[1], max[2]);
	m_vertices[17] = Vec3(max[0], max[1], max[2]);

	// 
	m_vertices[18] = Vec3(max[0], max[1], min[2]);
	m_vertices[19] = Vec3(min[0], max[1], min[2]);

	m_vertices[20] = Vec3(max[0], max[1], min[2]);
	m_vertices[21] = Vec3(max[0], min[1], min[2]);

	m_vertices[22] = Vec3(max[0], max[1], min[2]);
	m_vertices[23] = Vec3(max[0], max[1], max[2]);

	m_colors.resize(24);
	m_colors[0] = Vec4(1, 1, 0, 1);
	m_colors[1] = Vec4(1, 1, 0, 1);
	m_colors[2] = Vec4(1, 1, 0, 1);
	m_colors[3] = Vec4(1, 1, 0, 1);
	m_colors[4] = Vec4(1, 1, 0, 1);
	m_colors[5] = Vec4(1, 1, 0, 1);
	m_colors[6] = Vec4(1, 1, 0, 1);
	m_colors[7] = Vec4(1, 1, 0, 1);
	m_colors[8] = Vec4(1, 1, 0, 1);
	m_colors[9] = Vec4(1, 1, 0, 1);
	m_colors[10] = Vec4(1, 1, 0, 1);
	m_colors[11] = Vec4(1, 1, 0, 1);
	m_colors[12] = Vec4(1, 1, 0, 1);
	m_colors[13] = Vec4(1, 1, 0, 1);
	m_colors[14] = Vec4(1, 1, 0, 1);
	m_colors[15] = Vec4(1, 1, 0, 1);
	m_colors[16] = Vec4(1, 1, 0, 1);
	m_colors[17] = Vec4(1, 1, 0, 1);
	m_colors[18] = Vec4(1, 1, 0, 1);
	m_colors[19] = Vec4(1, 1, 0, 1);
	m_colors[20] = Vec4(1, 1, 0, 1);
	m_colors[21] = Vec4(1, 1, 0, 1);
	m_colors[22] = Vec4(1, 1, 0, 1);
	m_colors[23] = Vec4(1, 1, 0, 1);

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

	m_initialized = true;
}

void DrawNode::drawCube(const Vec3& min, const Vec3& max)
{
	m_vertices.resize(36);
	//
	m_vertices[0] = Vec3(min[0], min[1], min[2]);
	m_vertices[1] = Vec3(min[0], min[1], max[2]);
	m_vertices[2] = Vec3(max[0], min[1], max[2]);

	m_vertices[3] = Vec3(max[0], min[1], max[2]);
	m_vertices[4] = Vec3(max[0], min[1], min[2]);
	m_vertices[5] = Vec3(min[0], min[1], min[2]);

	//
	m_vertices[6] = Vec3(max[0], min[1], max[2]);
	m_vertices[7] = Vec3(max[0], min[1], min[2]);
	m_vertices[8] = Vec3(max[0], max[1], max[2]);

	m_vertices[9] = Vec3(max[0], max[1], max[2]);
	m_vertices[10] = Vec3(max[0], max[1], min[2]);
	m_vertices[11] = Vec3(max[0], min[1], min[2]);

	//
	m_vertices[12] = Vec3(max[0], max[1], max[2]);
	m_vertices[13] = Vec3(min[0], max[1], max[2]);
	m_vertices[14] = Vec3(min[0], max[1], min[2]);

	m_vertices[15] = Vec3(min[0], max[1], min[2]);
	m_vertices[16] = Vec3(max[0], max[1], min[2]);
	m_vertices[17] = Vec3(max[0], max[1], max[2]);

	//// 
	m_vertices[18] = Vec3(min[0], max[1], max[2]);
	m_vertices[19] = Vec3(min[0], max[1], min[2]);
	m_vertices[20] = Vec3(min[0], min[1], min[2]);

	m_vertices[21] = Vec3(min[0], min[1], min[2]);
	m_vertices[22] = Vec3(min[0], min[1], max[2]);
	m_vertices[23] = Vec3(min[0], max[1], max[2]);

	//
	m_vertices[24] = Vec3(min[0], min[1], max[2]);
	m_vertices[25] = Vec3(max[0], min[1], max[2]);
	m_vertices[26] = Vec3(min[0], max[1], max[2]);

	m_vertices[27] = Vec3(max[0], min[1], max[2]);
	m_vertices[28] = Vec3(max[0], max[1], max[2]);
	m_vertices[29] = Vec3(min[0], max[1], max[2]);

	//
	m_vertices[30] = Vec3(min[0], min[1], min[2]);
	m_vertices[31] = Vec3(max[0], min[1], min[2]);
	m_vertices[32] = Vec3(min[0], max[1], min[2]);

	m_vertices[33] = Vec3(max[0], min[1], min[2]);
	m_vertices[34] = Vec3(max[0], max[1], min[2]);
	m_vertices[35] = Vec3(min[0], max[1], min[2]);

	m_colors.resize(36);
	for (int i = 0; i < 36; ++i){
		m_colors[i] = Vec4(1, 1, 0, 1);
	}

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);

	m_initialized = true;
}

void DrawNode::drawSphere(const float raduis)
{

}

void DrawNode::drawScanner()
{

}