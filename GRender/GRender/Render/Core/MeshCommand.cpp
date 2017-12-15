#include "GL/glew.h"
#include "MeshCommand.h"
#include "GLProgramState.h"
#include "AABB.h"
#include "Texture2D.h"
#include "Mesh.h"

#include "Engine.h"
#include "Scene.h"
#include "Light.h"

USING_NAMESPACE_G

//render state
static bool   s_cullFaceEnabled = false;
static GLenum s_cullFace = 0;
static bool   s_depthTestEnabled = false;
static bool   s_depthWriteEnabled = false;

static int s_MAX = 8;

static const char          *s_dirLightUniformColorName = "G_directoinLightSourceColor";
static std::vector<Vec3> s_dirLightUniformColorValues;
static const char          *s_dirLightUniformDirName = "G_directionLightSourceDirection";
static std::vector<Vec3> s_dirLightUniformDirValues;

static const char          *s_pointLightUniformColorName = "G_pointLightSourceColor";
static std::vector<Vec3> s_pointLightUniformColorValues;
static const char          *s_pointLightUniformPositionName = "G_pointLightSourcePosition";
static std::vector<Vec3> s_pointLightUniformPositionValues;
static const char          *s_pointLightUniformRangeInverseName = "G_pointLightSourceRangeInverse";
static std::vector<float> s_pointLightUniformRangeInverseValues;

static const char          *s_spotLightUniformColorName = "G_spotLightSourceColor";
static std::vector<Vec3> s_spotLightUniformColorValues;
static const char          *s_spotLightUniformPositionName = "G_spotLightSourcePosition";
static std::vector<Vec3> s_spotLightUniformPositionValues;
static const char          *s_spotLightUniformDirName = "G_spotLightSourceDirection";
static std::vector<Vec3> s_spotLightUniformDirValues;
static const char          *s_spotLightUniformInnerAngleCosName = "G_spotLightSourceInnerAngleCos";
static std::vector<float> s_spotLightUniformInnerAngleCosValues;
static const char          *s_spotLightUniformOuterAngleCosName = "G_spotLightSourceOuterAngleCos";
static std::vector<float> s_spotLightUniformOuterAngleCosValues;
static const char          *s_spotLightUniformRangeInverseName = "G_spotLightSourceRangeInverse";
static std::vector<float> s_spotLightUniformRangeInverseValues;

static const char          *s_ambientLightUniformColorName = "G_ambientLightSourceColor";


MeshCommand::MeshCommand()
: m_callFaceEnabled(false)
, m_depthTestEnabled(false)
, m_depthWriteEnabled(false)
, m_isTextureRenderer(false)
, m_cullFace(GL_BACK)
, m_lightMask(LIGHTMASK)
, m_mesh(nullptr)
{
	m_type = RenderCommand::Type::MESH_COMMAND;
}

MeshCommand::~MeshCommand()
{

}

void MeshCommand::init(Mesh* mesh, const Mat4& mv, unsigned int priority)
{
	m_priorityOrder = priority;

	if (!mesh){
		return;
	}

	m_mesh = mesh;
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
	m_lightMask = lightMask;
}

/**execute*/
void MeshCommand::execute()
{
	if (!m_mesh){
		return;
	}

	// 1.
	applyRenderState();

	auto glProgram = m_mesh->getGLProgramState()->getGLProgram();
	glProgram->use();

	glProgram->setUniformsForBuiltins(m_mv);

	glProgram->setUniformLocationWith1i(glProgram->getUniformLocation("G_hasTexture"), m_mesh->hasTexture() ? 1 : 0 );
	
	int lightSize = Engine::getInstance()->getRunningScene()->getLigths().size();	
	glProgram->setUniformLocationWith1i(glProgram->getUniformLocation("G_hasLights"), lightSize);
	if (lightSize > 0){
		setLightUniforms();
	}

	if (m_mesh->hasTexture()){
		glProgram->setUniformLocationWith1i(glProgram->getUniformLocation("G_texture0"), 0);
		glActiveTexture(GL_TEXTURE0); 
		glBindTexture(GL_TEXTURE_2D, m_mesh->getTexture2D()->getName());
	}

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

	restoreRenderState();
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
	Engine *engine = Engine::getInstance();
	auto scene = engine->getRunningScene();

	auto &lights = scene->getLigths();
	auto glProgram = m_mesh->getGLProgramState()->getGLProgram();

	// 是否有法线
	if (m_mesh->hasNormal()){

		resetLightUniforms();

		GLint enabledDirLightNum = 0;
		GLint enabledPointLightNum = 0;
		GLint enabledSpotLightNum = 0;
		Vec3 ambientColor = Vec3(0.1,0.1,0.1);

		for (const auto& light : lights){
			bool useLight = light->isEnable() &&
				(light->getLightFlag() & m_lightMask);

			if (useLight){
				float intensity = light->getIntensity();
				switch (light->getLightType())
				{
				case LightType::DIRECTION:
				{
					if (enabledDirLightNum < s_MAX){
						auto dirLight = static_cast<DirectionLight*>(light);
						Vec3 dir = dirLight->getDirectionInWorld();
						dir.normalize();

						const Color4& col = dirLight->getDisplayColor();
						s_dirLightUniformColorValues[enabledDirLightNum] = Vec3(col[0], col[1], col[2]);
						s_dirLightUniformDirValues[enabledDirLightNum] = dir;
						enabledDirLightNum++;
					}
				}
					break;
				case LightType::POINT:
				{
					if (enabledPointLightNum < s_MAX){
						auto pointLight = static_cast<PointLight*>(light);

						Mat4 mat = pointLight->getNodeToWorldTransform();
						const Color4& col = pointLight->getDisplayColor();

						s_pointLightUniformColorValues[enabledPointLightNum] =
							Vec3(col[0] * intensity, col[1] * intensity, col[2] * intensity);
						s_pointLightUniformPositionValues[enabledPointLightNum] =
							Vec3(mat.data()[12], mat.data()[13], mat.data()[14]);
						s_pointLightUniformRangeInverseValues[enabledPointLightNum] =
							1.0f / pointLight->getRange();
						enabledPointLightNum++;
					}
				}
					break;
				case LightType::SPOT:
				{
					if (enabledSpotLightNum < s_MAX){
						auto spotLight = static_cast<SpotLight*>(light);
						Vec3 dir = spotLight->getDirectionInWorld();
						dir.normalize();

						Mat4 mat = spotLight->getNodeToWorldTransform();
						const Color4& col = spotLight->getDisplayColor();

						s_spotLightUniformColorValues[enabledSpotLightNum] =
							Vec3(col[0] * intensity, col[1] * intensity, col[2] * intensity);
						s_spotLightUniformPositionValues[enabledSpotLightNum] =
							Vec3(mat.data()[12], mat.data()[13], mat.data()[14]);
						s_spotLightUniformDirValues[enabledSpotLightNum] = dir;
						s_spotLightUniformInnerAngleCosValues[enabledSpotLightNum] = spotLight->getCosInnerAngle();
						s_spotLightUniformOuterAngleCosValues[enabledSpotLightNum] = spotLight->getCosOuterAngle();
						s_spotLightUniformRangeInverseValues[enabledSpotLightNum] = 1.0f / spotLight->getRange();

						enabledSpotLightNum++;
					}
				}
					break;
				case LightType::AMBIENT:
				{
					auto ambient = static_cast<AmbientLight*>(light);
					const Color4& col = ambient->getDisplayColor();
					ambientColor += Vec3(col[0] * intensity, col[1] * intensity, col[2] * intensity);
				}
					break;
				default:
					break;
				}
			}
		}

		// direction
		glProgram->setUniformLocationWith3fv(glProgram->getUniformLocation(s_dirLightUniformColorName),
			(GLfloat*)&s_dirLightUniformColorValues[0], s_dirLightUniformColorValues.size());
		glProgram->setUniformLocationWith3fv(glProgram->getUniformLocation(s_dirLightUniformDirName),
			(GLfloat*)&s_dirLightUniformDirValues[0], s_dirLightUniformDirValues.size());

		// point
		glProgram->setUniformLocationWith3fv(glProgram->getUniformLocation(s_pointLightUniformColorName),
			(GLfloat*)&s_pointLightUniformColorValues[0], s_pointLightUniformColorValues.size());
		glProgram->setUniformLocationWith3fv(glProgram->getUniformLocation(s_pointLightUniformPositionName),
			(GLfloat*)&s_pointLightUniformPositionValues[0], s_pointLightUniformPositionValues.size());
		glProgram->setUniformLocationWith1fv(glProgram->getUniformLocation(s_pointLightUniformRangeInverseName),
			(GLfloat*)&s_pointLightUniformRangeInverseValues[0], s_pointLightUniformRangeInverseValues.size());

		// spot
		glProgram->setUniformLocationWith3fv(glProgram->getUniformLocation(s_spotLightUniformColorName),
			(GLfloat*)&s_spotLightUniformColorValues[0], s_spotLightUniformColorValues.size());
		glProgram->setUniformLocationWith3fv(glProgram->getUniformLocation(s_spotLightUniformPositionName),
			(GLfloat*)&s_spotLightUniformPositionValues[0], s_spotLightUniformPositionValues.size());
		glProgram->setUniformLocationWith3fv(glProgram->getUniformLocation(s_spotLightUniformDirName),
			(GLfloat*)&s_spotLightUniformDirValues[0], s_spotLightUniformDirValues.size());
		glProgram->setUniformLocationWith1fv(glProgram->getUniformLocation(s_spotLightUniformInnerAngleCosName),
			(GLfloat*)&s_spotLightUniformInnerAngleCosValues[0], s_spotLightUniformInnerAngleCosValues.size());
		glProgram->setUniformLocationWith1fv(glProgram->getUniformLocation(s_spotLightUniformOuterAngleCosName),
			(GLfloat*)&s_spotLightUniformOuterAngleCosValues[0], s_spotLightUniformOuterAngleCosValues.size());
		glProgram->setUniformLocationWith1fv(glProgram->getUniformLocation(s_spotLightUniformRangeInverseName),
			(GLfloat*)&s_spotLightUniformRangeInverseValues[0], s_spotLightUniformRangeInverseValues.size());

		//ambient
		glProgram->setUniformLocationWith3f(glProgram->getUniformLocation(s_ambientLightUniformColorName),
			ambientColor[0], ambientColor[1], ambientColor[2]);

		//G::log("%f, %f, %f \n", ambientColor[0], ambientColor[1], ambientColor[2]);
	}
	else
	{
		Vec3 ambient(Vec3::Zero());
		bool hasAmbient = false;

		for (const auto& light : lights){
			if (light->getLightType() == LightType::AMBIENT){
				bool useLight = light->isEnable() &&
					((unsigned int)light->getLightFlag() & m_lightMask);
				if (useLight){
					hasAmbient = true;

					const Color4& col = light->getDisplayColor();
					ambient[0] += col[0] * light->getIntensity();
					ambient[1] += col[1] * light->getIntensity();
					ambient[2] += col[2] * light->getIntensity();
				}
			}
		}

		glProgram->setUniformLocationWith3f(glProgram->getUniformLocation("G_ambientLightSourceColor"),
			ambient[0], ambient[1], ambient[2]);
	}
}

void MeshCommand::resetLightUniforms()
{
	s_dirLightUniformColorValues.assign(s_MAX, Vec3::Zero());
	s_dirLightUniformDirValues.assign(s_MAX, Vec3::Zero());

	s_pointLightUniformColorValues.assign(s_MAX, Vec3::Zero());
	s_pointLightUniformPositionValues.assign(s_MAX, Vec3::Zero());
	s_pointLightUniformRangeInverseValues.assign(s_MAX, 0.0f);

	s_spotLightUniformColorValues.assign(s_MAX, Vec3::Zero());
	s_spotLightUniformPositionValues.assign(s_MAX, Vec3::Zero());
	s_spotLightUniformDirValues.assign(s_MAX, Vec3::Zero());
	s_spotLightUniformInnerAngleCosValues.assign(s_MAX, 0.0f);
	s_spotLightUniformOuterAngleCosValues.assign(s_MAX, 0.0f);
	s_spotLightUniformRangeInverseValues.assign(s_MAX, 0.0f);
}