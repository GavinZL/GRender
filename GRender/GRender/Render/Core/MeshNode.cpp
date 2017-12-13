#include "GL/glew.h"
#include "MeshNode.h"

#include "Renderer.h"
#include "Mesh.h"
#include "IOManager.h"

#include "GLProgram.h"
#include "GLProgramState.h"
#include "GLProgramCache.h"

#include "Image.h"
#include "Texture2D.h"

#include "Engine.h"
#include "Scene.h"
#include "Light.h"
#include "DrawNode.h"

#include "AABB.h"

#include "../Comm/Utils.h"

USING_NAMESPACE_G;

MeshNode::MeshNode()
	: m_mesh(nullptr)
	, m_lightMask(-1)
	, m_shaderUsingLight(false)
	, m_aabbVisiable(true)
	, m_boxNode(nullptr)
{
	m_nodeFlagMask = _MESH;
}

MeshNode::MeshNode(const std::string& filePath)
	: m_mesh(nullptr)
{
	m_nodeFlagMask = _MESH;
	initMesh(filePath);
}

MeshNode::~MeshNode()
{

}

/**initialize mesh with file path*/
bool MeshNode::initMesh(const std::string& modelPath)
{
	std::vector<Vec3> vs;
	std::vector<Vec3> ns;
	std::vector<Vec4> cs;
	std::vector<Vec2> ts;
	std::vector<unsigned int> is;

	std::string texturePath;

	IOManager::getInstance()->loadModelFromFile(modelPath,
		vs, ns, cs, ts, is, texturePath);

	return initMesh(vs, ns, cs, ts, is,texturePath);
}

bool MeshNode::initMesh(const std::vector<Vec3>& position,
	const std::vector<Vec3>& normals,
	const std::vector<Vec4>& colors,
	const std::vector<Vec2>& texs,
	const std::vector<unsigned int>& indics,
	const std::string& texturePath)
{
	if (nullptr != m_mesh){
		delete m_mesh;
		m_mesh = nullptr;
	}

	m_mesh = Mesh::create(position, normals, colors, texs, indics,texturePath);

	genGLProgramState(true);

	// 添加一个box
	if (m_mesh && m_mesh->getAABB()){
		m_aabb = m_mesh->getAABB();

		m_boxNode = new (std::nothrow)DrawNode();
		m_boxNode->init(DrawNode::Type::BOX, nullptr);
		m_boxNode->drawBox(m_aabb->_min, m_aabb->_max, 2);
		this->addChild(m_boxNode);
	}
	return true;
}


void MeshNode::hideBoundingBox(bool b)
{
	m_aabbVisiable = b;
	if (m_boxNode != nullptr){
		m_boxNode->setVisible(b);
	}
}

void MeshNode::setBoundingBoxColor(const Color3& color)
{
	m_aabbVisiable = true;
	if (nullptr != m_boxNode){
		m_boxNode->setDisplayColor(color);
		m_boxNode->setVisible(m_aabbVisiable);
	}
}

void MeshNode::updateBoundingBox()
{
	if (m_aabb && m_boxNode){
		m_boxNode->updateBox(m_aabb->_min, m_aabb->_max);
	}
}

void MeshNode::draw(Renderer* renderer, const Mat4& transform, unsigned int flags)
{
	if (!m_mesh){
		return;
	}

	if (!m_mesh->isVisible()){
		return;
	}
	
	Color4 c = getDisplayColor();

	const auto& lights = Engine::getInstance()->getRunningScene()->getLigths();
	bool usingLigth = false;
	for (const auto& light : lights){
		usingLigth = ((unsigned int)light->getLightFlag() & m_lightMask) > 0;
		if (usingLigth){
			break;
		}
	}

	// shader
	if (usingLigth != m_shaderUsingLight){
		genGLProgramState(usingLigth);
	}

	auto programSt = m_mesh->getGLProgramState();
	auto meshCommand = m_mesh->getMeshCommand();

	bool isTransparent = false;//(m_mesh->isTransparent() || c[3] < 1.0f);

	meshCommand->init(m_mesh, transform, m_priority);
	meshCommand->setLightMask(m_lightMask);
	meshCommand->setDisplayColor(c);
	meshCommand->setTransparent(isTransparent);

	renderer->addCommand(meshCommand);	
}


void MeshNode::genGLProgramState(bool useLight)
{
	if (m_mesh == nullptr){
		G::log("Error : MeshNode::genGLProgramState()  mesh is nullptr .");
		return;
	}

	m_shaderUsingLight = useLight;

	GLProgram *glProgram = nullptr;
	const char* shader = nullptr;

	// 目前默认 有 顶点，法线，颜色
	//if (m_mesh->hasTexture()){
		shader = GLProgram::SHADER_NAME_POSITION_NORMAL_TEXTURE_COLOR;
	//}
	//else
	//{
	//	shader = GLProgram::SHADER_NAME_POSITION_NORMAL_COLOR;
	//}

	m_mesh->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(shader));
}