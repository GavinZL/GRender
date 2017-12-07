#include "GL/glew.h"
#include "MeshCommand.h"
#include "GLProgramState.h"
#include "AABB.h"
#include "Texture2D.h"
#include "Mesh.h"

USING_NAMESPACE_G

//render state
static bool   s_cullFaceEnabled = false;
static GLenum s_cullFace = 0;
static bool   s_depthTestEnabled = false;
static bool   s_depthWriteEnabled = false;

MeshCommand::MeshCommand()
: m_textureId(0)
, m_glProgramState(nullptr)
, m_vertexBuffer(0)
, m_colorBuffer(0)
, m_normalBuffer(0)
, m_indexBuffer(0)
, m_indexCount(0)
, m_callFaceEnabled(false)
, m_depthTestEnabled(false)
, m_depthWriteEnabled(false)
, m_isTextureRenderer(false)
, m_cullFace(GL_BACK)
, m_lightMask(0)
, m_mesh(nullptr)
{
	m_type = RenderCommand::Type::MESH_COMMAND;
}

MeshCommand::~MeshCommand()
{

}

/**Initialize*/
void MeshCommand::init(GLuint textureId, GLProgramState* glProgramState, BlendFunc blendType,
	GLuint vertexBuffer, GLuint indexBuffer, GLenum primitive, GLenum indexType,
	long long indexCount, const Mat4& mv)
{
	m_textureId = textureId;
	m_glProgramState = glProgramState;
	m_vertexBuffer = vertexBuffer;
	m_indexBuffer = indexBuffer;
	m_primitive = primitive;
	m_indexFormat = indexType;
	m_indexCount = indexCount;
	m_mv = mv;

	m_isTextureRenderer = true;
}

void MeshCommand::init(GLProgramState* glProgramState, GLuint vertexBuffer, GLuint vertexColor,
	GLuint indexBuffer, GLenum primitive, GLenum indexType, long long indexCount, const Mat4& mv)
{
	m_glProgramState = glProgramState;
	m_vertexBuffer = vertexBuffer;
	m_colorBuffer = vertexColor;
	m_indexBuffer = indexBuffer;
	m_primitive = primitive;
	m_indexFormat = indexType;
	m_indexCount = indexCount;
	m_mv = mv;

	m_isTextureRenderer = false;
}

void MeshCommand::init(Mesh* mesh, const Mat4& mv)
{
	if (!mesh){
		return;
	}

	m_mesh = mesh;

	m_glProgramState = mesh->getGLProgramState();

	m_vertexBuffer = mesh->getVertexBuffer();
	
	if (mesh->hasColor()){
		m_colorBuffer = mesh->getColorBuffer();
	}

	if (mesh->hasNormal()){
		m_normalBuffer = mesh->getNormalBuffer();
	}

	if (mesh->hasIndics()){
		m_indexBuffer = mesh->getIndexBuffer();
	}
	
	//if (mesh->hasTexture()){
	//	m_textureId = mesh->getTexture2D()->getName();
	//}

	m_mv = mv;
}

/**GL states*/
void MeshCommand::setCullFaceEnabled(bool en)
{
	m_callFaceEnabled = en;
}

void MeshCommand::setCullface(GLenum cuffFace)
{
	m_cullFace = cuffFace;
}


void MeshCommand::setDepthTestEnabled(bool en)
{
	m_depthTestEnabled = en;
}

void MeshCommand::setDepthWriteEnabled(bool en)
{
	m_depthWriteEnabled = en;
}

void MeshCommand::setDisplayColor(const Vec4& color)
{

}

void MeshCommand::setLightMask(unsigned int lightMask)
{

}

/**execute*/
void MeshCommand::execute()
{
	if (!m_mesh){
		return;
	}

	// 1.
	//applyRenderState();

	auto glProgram = m_mesh->getGLProgramState()->getGLProgram();
	glProgram->use();

	glProgram->setUniformsForBuiltins(m_mv);

	glProgram->setUniformLocationWith4f(glProgram->getUniformLocation(GLProgram::UNIFORM_NAME_DISPLAYCOLOR), 1.0f, 1.0f, 1.0f, 1.0f);
	glProgram->setUniformLocationWith4f(glProgram->getUniformLocation(GLProgram::UNIFORM_NAME_LIGHTCOLOR), 1.0f, 1.0f, 1.0f, 1.0f);
	glProgram->setUniformLocationWith4f(glProgram->getUniformLocation(GLProgram::UNIFORM_NAME_AMBIENT_COLOR), 1.0f, 1.0f, 1.0f, 1.0f);
	glProgram->setUniformLocationWith3f(glProgram->getUniformLocation(GLProgram::UNIFORM_NAME_LIGHTPOSITION), 10.0f, 10.0f, 10.0f);

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, m_mesh->getVertexBuffer());
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	if (m_mesh->hasColor()){
		glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, m_mesh->getColorBuffer());
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_COLOR, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	}

	if (m_mesh->hasNormal()){
		glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_NORMAL);
		glBindBuffer(GL_ARRAY_BUFFER, m_mesh->getNormalBuffer());
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	}

	if (m_mesh->hasTexture()){
		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_TEXTURE_2D, m_mesh->getTexture2D()->getName());

		glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, m_mesh->getTextureBuffer());
		glVertexAttribPointer(GLProgram::VERTEX_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	}

	if (m_mesh->hasIndics()){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh->getIndexBuffer());
	}

	if (m_mesh->hasIndics()){
		glDrawElements(m_mesh->getPrimitive(), m_mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
	}
	else{
		glDrawArrays(m_mesh->getPrimitive(), 0, m_mesh->getVertexCount());
	}

	glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_POSITION);
	if (m_mesh->hasNormal()){
		glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_NORMAL);
	}
	if (m_mesh->hasColor()){
		glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_COLOR);
	}
	if (m_mesh->hasTexture()){
		glDisableVertexAttribArray(GLProgram::VERTEX_ATTRIBUTE_TEXTURE0);
	}

	//restoreRenderState();
}

/**apply render states*/
void MeshCommand::applyRenderState()
{
	if (m_callFaceEnabled && !s_cullFaceEnabled){
		glEnable(GL_CULL_FACE);
		s_cullFaceEnabled = true;
	}

	if (s_cullFace != m_cullFace){
		glCullFace(m_cullFace);
		s_cullFace = m_cullFace;
	}

	if (m_depthTestEnabled && !s_depthTestEnabled){
		glEnable(GL_DEPTH_TEST);
		s_depthTestEnabled = true;
	}

	if (m_depthWriteEnabled && !s_depthWriteEnabled){
		glDepthMask(GL_TRUE);
		s_depthWriteEnabled = true;
	}
}

/**restore all states*/
void MeshCommand::restoreRenderState()
{
	if (s_cullFaceEnabled){
		glDisable(GL_CULL_FACE);
		s_cullFaceEnabled = false;
	}

	if (s_depthTestEnabled){
		glDisable(GL_DEPTH_TEST);
		s_depthTestEnabled = false;
	}

	if (s_depthWriteEnabled){
		glDepthMask(GL_FALSE);
		s_depthWriteEnabled = false;
	}

	s_cullFace = 0;
}

/**set lights uniforms*/
void MeshCommand::setLightUniforms()
{

}

void MeshCommand::resetLightUniforms()
{

}