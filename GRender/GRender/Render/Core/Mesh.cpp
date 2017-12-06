#include "GL/glew.h"
#include "Mesh.h"
#include "GLProgramState.h";
#include "AABB.h"
#include "Texture2D.h"
#include "../Comm/Utils.h"

USING_NAMESPACE_G;

Mesh::Mesh()
	: m_isTextureMesh(false)
	, m_visible(true)
	, m_transparent(false)
	, m_name("")
	, m_glProgramState(nullptr)
	, m_aabb(nullptr)
	, m_texture(nullptr)
	, m_vertexBuffer(0)
	, m_colorBuffer(0)
	, m_indexBuffer(0)
	, m_textureBuffer(0)
	, m_primitive(GL_TRIANGLES)
{

}

Mesh::~Mesh()
{

}


bool Mesh::init(const std::vector<Vec3>& position,
	const std::vector<Vec3>& normals,
	const std::vector<Vec4>& colors,
	const std::vector<Vec2>& texs,
	const std::vector<unsigned int>& indics)
{
	if (position.size() <= 0){
		G::log("Error : Mesh::create() position.size() <= 0 \n");
		return false;
	}

	m_vertics.resize(position.size());
	std::copy(position.begin(), position.end(), m_vertics.begin());

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertics.size(), &m_vertics[0], GL_STATIC_DRAW);

	if (normals.size() > 0){
		m_normals.resize(normals.size());
		std::copy(normals.begin(), normals.end(), m_normals.begin());

		glGenBuffers(1, &m_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW);
	}

	if (colors.size() > 0){
		m_colors.resize(colors.size());
		std::copy(colors.begin(), colors.end(), m_colors.begin());

		glGenBuffers(1, &m_colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_STATIC_DRAW);
	}

	if (indics.size() > 0){
		m_indics.resize(indics.size());
		std::copy(indics.begin(), indics.end(), m_indics.begin());

		glGenBuffers(1, &m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indics.size(), &m_indics[0], GL_STATIC_DRAW);
	}

	if (texs.size() > 0){
		m_textures.resize(texs.size());
		std::copy(texs.begin(), texs.end(), m_textures.begin());

		glGenBuffers(1, &m_textureBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_textureBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * m_textures.size(), &m_textures[0], GL_STATIC_DRAW);
	}

	calculateAABB(m_vertics);

	m_isTextureMesh = false;
	return true;
}

void Mesh::calculateAABB(const std::vector<Vec3>& vertics)
{
	if (m_aabb == nullptr){
		m_aabb = new AABB();
	}

	m_aabb->updateMinMax(&vertics[0], vertics.size());
}


/**create mesh with color*/
Mesh* Mesh::create(const std::vector<Vec3>& position,
	const std::vector<Vec3>& normals,
	const std::vector<Vec4>& colors,
	const std::vector<Vec2>& texs,
	const std::vector<unsigned int>& indics)
{
	auto mesh = new (std::nothrow) Mesh();
	if (mesh && mesh->init(position, normals, colors, texs, indics)){
		return mesh;
	}

	return nullptr;
}

void Mesh::setTexture2D( Texture2D* tex)
{ 
	m_texture = tex; 
}