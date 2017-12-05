
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

	_positionID = getGLProgram()->getAttributeLocation("G_position");
	_colorID = getGLProgram()->getAttributeLocation("G_color");

	G::log("P : %d , C : %d \n", _positionID, _colorID);
	G::log("-------------------------------------\n");

	_positionID = glGetAttribLocation(getGLProgram()->getProgram(), "G_position");
	_colorID = glGetAttribLocation(getGLProgram()->getProgram(), "G_color");

	G::log("P : %d , C : %d \n", _positionID, _colorID);

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
	/*
	GLProgram *program = new (std::nothrow) GLProgram;

	bool b = program->initProgramWithFile("shader/position_color_vert.shader", "shader/position_color_frag.shader");
	bool c = program->link();
	
	_programId = program->getProgram();

	_positionID = glGetAttribLocation(_programId, "G_position");

	float arr[] = {
		0.5, 0.5, 0, 1, 0.5, 0,
		1, 0.5, 0, 0.8, 0.5, 0.2,
		1, 0.5, 0, 0.8, 0.5, -0.2,

		0, 0, 0, 0, 0.5, 0,
		0, 0.5, 0, 0.2, 0.8, 0,
		0, 0.5, 0, -0.2, 0.8, 0,

		0, 0, 0, 0, 0, 0.5,
		0, 0, 0.5, 0.2, 0, 0.8,
		0, 0, 0.5, -0.2, 0, 0.8
	};

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arr), &arr[0], GL_STATIC_DRAW);


	m_initialized = true;
	*/
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

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glDrawArrays(GL_LINES, 0, m_vertices.size());

	glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_COLOR);
	glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_POSITION);
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

void DrawNode::drawBox(const float x_duration, const float y_duration, const float z_duration, const float lineSize)
{

}

void DrawNode::drawCube(const float x_duration, const float y_duration, const float z_duration)
{

}

void DrawNode::drawSphere(const float raduis)
{

}

void DrawNode::drawScanner()
{

}