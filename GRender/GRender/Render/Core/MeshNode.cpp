#include "MeshNode.h"

#include "Renderer.h"
#include "Mesh.h"
#include "IOManager.h"

#include "GLProgram.h"
#include "GLProgramState.h"
#include "GLProgramCache.h"

#include "../Comm/Utils.h"

USING_NAMESPACE_G;

MeshNode::MeshNode()
	: m_mesh(nullptr)
{

}

MeshNode::MeshNode(const std::string& filePath)
	: m_mesh(nullptr)
{
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

	return initMesh(vs, ns, cs, ts, is);
}

bool MeshNode::initMesh(const std::vector<Vec3>& position,
	const std::vector<Vec3>& normals,
	const std::vector<Vec4>& colors,
	const std::vector<Vec2>& texs,
	const std::vector<unsigned int>& indics)
{
	if (position.size() == 0){
		return false;
	}

	if (nullptr != m_mesh){
		delete m_mesh;
		m_mesh = nullptr;
	}

	m_mesh = Mesh::create(position, normals, colors, texs, indics);

	// 
	genGLProgramState(true);

	return true;
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

	auto programSt = m_mesh->getGLProgramState();
	auto meshCommand = m_mesh->getMeshCommand();

	// todo texture..

	//
	bool isTransparent = false;//(m_mesh->isTransparent() || c[3] < 1.0f);

	meshCommand->init(m_mesh, transform);
	meshCommand->setLightMask(0);
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

	GLProgram *glProgram = nullptr;
	const char* shader = nullptr;

	// 目前默认 有 顶点，法线，颜色
	if (m_mesh->hasTexture()){
		shader = GLProgram::SHADER_NAME_POSITION_NORMAL_TEXTURE_COLOR;
	}
	else
	{
		shader = GLProgram::SHADER_NAME_POSITION_NORMAL_COLOR;
	}

	if (shader){
		glProgram = GLProgramCache::getInstance()->getGLProgram(shader);
	}

	auto programSt = GLProgramState::create(glProgram);

	if (programSt){
		m_mesh->setGLProgramState(programSt);
	}
}