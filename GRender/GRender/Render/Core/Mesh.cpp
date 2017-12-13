#include "GL/glew.h"
#include "Mesh.h"
#include "GLProgramState.h";
#include "AABB.h"
#include "Texture2D.h"
#include "../Comm/Utils.h"

#include "Image.h"

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
	, m_vertexCount(0)
	, m_indicsCount(0)
{

}

Mesh::~Mesh()
{

}

bool Mesh::init(const std::vector<Vec3>& position,
	const std::vector<Vec3>& normals,
	const std::vector<Vec4>& colors,
	const std::vector<Vec2>& texs,
	const std::vector<unsigned int>& indics,
	const std::string& texturePath)
{
	if (position.size() <= 0){
		G::log("Error : Mesh::create() position.size() <= 0 \n");
		return false;
	}

	if (!texturePath.empty()){
		Image* img = new (std::nothrow)Image();
		img->initWithImageFile(texturePath);

		m_texture = new(std::nothrow) Texture2D();
		m_texture->initWithImage(img);
	}

	m_vertics.resize(position.size());
	m_verticsBak.resize(position.size());
	std::copy(position.begin(), position.end(), m_vertics.begin());
	std::copy(position.begin(), position.end(), m_verticsBak.begin());

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_vertics.size(), &m_vertics[0], GL_STATIC_DRAW);

	m_vertexCount = m_vertics.size();

	if (normals.size() > 0){
		m_normals.resize(normals.size());
		m_normalsBak.resize(normals.size());
		std::copy(normals.begin(), normals.end(), m_normals.begin());
		std::copy(normals.begin(), normals.end(), m_normalsBak.begin());

		glGenBuffers(1, &m_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_normals.size(), &m_normals[0], GL_STATIC_DRAW);
	}

	if (colors.size() > 0){
		m_colors.resize(colors.size());
		m_colorsBak.resize(colors.size());
		std::copy(colors.begin(), colors.end(), m_colors.begin());
		std::copy(colors.begin(), colors.end(), m_colorsBak.begin());

		glGenBuffers(1, &m_colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_DYNAMIC_DRAW);
	}

	if (indics.size() > 0){
		m_indics.resize(indics.size());
		m_indicsBak.resize(indics.size());
		std::copy(indics.begin(), indics.end(), m_indics.begin());
		std::copy(indics.begin(), indics.end(), m_indicsBak.begin());

		glGenBuffers(1, &m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indics.size(), &m_indics[0], GL_STATIC_DRAW);

		m_indicsCount = m_indics.size();
	}

	if (texs.size() > 0){
		m_textures.resize(texs.size());
		m_texturesBak.resize(texs.size());
		std::copy(texs.begin(), texs.end(), m_textures.begin());
		std::copy(texs.begin(), texs.end(), m_texturesBak.begin());

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
	const std::vector<unsigned int>& indics,
	const std::string& texturePath)
{
	auto mesh = new (std::nothrow) Mesh();
	if (mesh && mesh->init(position, normals, colors, texs, indics,texturePath)){
		return mesh;
	}

	return nullptr;
}

void Mesh::setTexture2D( Texture2D* tex)
{ 
	m_texture = tex; 
}

//  [12/12/2017 Administrator]

// ##修改指定索引的颜色值
void Mesh::changeColor(unsigned int index, const Vec4& color)
{
	if (m_displayColors.size() == 0){
		resetColor();
	}

	if (index < 0 || index > m_displayColors.size()){
		return;
	}

	m_displayColors[index] = color;

	// 表示可以用于删除的顶点索引
	m_modifyVerticsIndex.push_back(index);
}

void Mesh::flashColor()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * m_displayColors.size(), &m_displayColors[0]);
}

void Mesh::inverseColor(bool forward)
{
	std::vector<bool> mask(m_verticsBak.size(),!forward);
	for (int i = 0, cnt = m_modifyVerticsIndex.size(); i < cnt; ++i){
		mask[m_modifyVerticsIndex[i]] = forward;
	}

	for (int i = 0, cnt = m_colorsBak.size(); i < cnt; ++i){
		if (mask[i]){
			m_displayColors[i] = m_colorsBak[i];
		}
		else{
			m_displayColors[i] = Vec4(1.0, 0.0, 0.0, 0.0);	// 此处指定了， 否则需要存储颜色值
		}
	}
}

// ##恢复原始颜色值
void Mesh::resetColor()
{
	m_displayColors.resize(m_colorsBak.size());
	std::copy(m_colorsBak.begin(), m_colorsBak.end(), m_displayColors.begin());

	m_modifyVerticsIndex.clear();
}

//  [12/13/2017 Administrator]
// 删点操作
void Mesh::deleteVertics()
{
	// 删除一次点后， 再进行第二次删除， 默认第一次的删除操作为用户确认状态， 顾如果再进行第二次删除后 ， 无法恢复第一次删除前的状态
	// 删点恢复 只能恢复当前删除操作的 前一次状态， 
	if (m_verticsBak.size() < m_vertics.size()){
		m_vertics.resize(m_verticsBak.size());
		std::copy(m_verticsBak.begin(), m_verticsBak.end(), m_vertics.begin());

		if (m_normals.size() > 0){
			m_normals.resize(m_normalsBak.size());
			std::copy(m_normalsBak.begin(), m_normalsBak.end(), m_normals.begin());
		}
		if (m_colors.size() > 0){
			m_colors.resize(m_colorsBak.size());
			std::copy(m_colorsBak.begin(), m_colorsBak.end(), m_colors.begin());
		}
		if (m_textures.size() > 0){
			m_textures.resize(m_texturesBak.size());
			std::copy(m_texturesBak.begin(), m_texturesBak.end(), m_textures.begin());
		}
		if (m_indics.size() > 0){
			m_indics.resize(m_indicsBak.size());
			std::copy(m_indicsBak.begin(), m_indicsBak.end(), m_indics.begin());
		}
	}

	//删除选中顶点
	m_verticsBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_normalsBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_texturesBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_colorsBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_indicsBak.clear();

	// ##删除前后， 索引的变化对应
	std::vector<unsigned int> m_mapIndics(m_vertics.size(), -1);

	std::vector<bool> mask(m_vertics.size(), false);
	for (int i = 0, cnt = m_modifyVerticsIndex.size(); i < cnt; ++i){
		mask[m_modifyVerticsIndex[i]] = true;
	}


	int icnt = 0;
	for (int i = 0, cnt = m_vertics.size(); i < cnt; ++i){
		if (!mask[i]){
			m_verticsBak[icnt] = m_vertics[i];
			if (hasNormal())
				m_normalsBak[icnt] = m_normals[i];
			if (hasTexture())
				m_texturesBak[icnt] = m_textures[i];
			if (hasColor())
				m_colorsBak[icnt] = m_colors[i];
		
			m_mapIndics[i] = icnt;
			icnt++;
		}
	}

	if (hasIndics()){
		// 修改三角索引
		for (int i = 0, cnt = m_indics.size(); i < cnt; i += 3){
			// 如果一个三角形 顶点都没有被标记, 则保留
			if (!(mask[m_indics[i + 0]] || mask[m_indics[i + 1]] || mask[m_indics[i + 2]]))
			{
				m_indicsBak.push_back(m_mapIndics[m_indics[i + 0]]);
				m_indicsBak.push_back(m_mapIndics[m_indics[i + 1]]);
				m_indicsBak.push_back(m_mapIndics[m_indics[i + 2]]);
			}
		}

	}
	m_vertexCount = m_verticsBak.size();
	m_indicsCount = m_indicsBak.size();
	
	flashBuffer(); 
	resetColor();
}

void Mesh::resetVertics()
{
	m_verticsBak.resize(m_vertics.size());
	std::copy(m_vertics.begin(), m_vertics.end(), m_verticsBak.begin());

	if (m_normals.size() > 0){
		m_normalsBak.resize(m_normals.size());
		std::copy(m_normals.begin(), m_normals.end(), m_normalsBak.begin());
	}

	if (m_colors.size() > 0){
		m_colorsBak.resize(m_colors.size());
		std::copy(m_colors.begin(), m_colors.end(), m_colorsBak.begin());
	}

	if (m_textures.size() > 0){
		m_texturesBak.resize(m_textures.size());
		std::copy(m_textures.begin(), m_textures.end(), m_texturesBak.begin());
	}

	if (m_indics.size() > 0){
		m_indicsBak.resize(m_indics.size());
		std::copy(m_indics.begin(), m_indics.end(), m_indicsBak.begin());
	}

	//glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * m_vertics.size(), &m_vertics[0]);

	if (m_normals.size() > 0){
		//glGenBuffers(1, &m_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(Vec3) * m_normals.size(), &m_normals[0]);
	}

	if (m_colors.size() > 0){

		//glGenBuffers(1, &m_colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * m_colors.size(), &m_colors[0]);
	}

	if (m_indics.size() > 0){

		//glGenBuffers(1, &m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_indics.size(), &m_indics[0]);
	}

	if (m_textures.size() > 0){

		//glGenBuffers(1, &m_textureBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_textureBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec2) * m_textures.size(), &m_textures[0]);

	}

	m_vertexCount = m_vertics.size();
	m_indicsCount = m_indics.size();


	calculateAABB(m_vertics);

	resetColor();
}

void Mesh::flashBuffer()
{
	//glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * m_verticsBak.size(), &m_verticsBak[0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_verticsBak.size(), &m_verticsBak[0], GL_STATIC_DRAW);

	if (m_normalsBak.size() > 0){
		//glGenBuffers(1, &m_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(Vec3) * m_normalsBak.size(), &m_normalsBak[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * m_normalsBak.size(), &m_normalsBak[0], GL_STATIC_DRAW);
	}

	if (m_colorsBak.size() > 0){

		//glGenBuffers(1, &m_colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * m_colorsBak.size(), &m_colorsBak[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colorsBak.size(), &m_colorsBak[0], GL_STATIC_DRAW);
	}

	if (m_indicsBak.size() > 0){

		//glGenBuffers(1, &m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_indicsBak.size(), &m_indicsBak[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int) * m_indicsBak.size(), &m_indicsBak[0], GL_STATIC_DRAW);
	}

	if (m_texturesBak.size() > 0){

		//glGenBuffers(1, &m_textureBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_textureBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec2) * m_texturesBak.size(), &m_texturesBak[0]);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * m_texturesBak.size(), &m_texturesBak[0], GL_STATIC_DRAW);
	}

	calculateAABB(m_verticsBak);
}