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
{

}

Mesh::~Mesh()
{

}

//#include <QImage>
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

		//QImage image(texturePath.c_str());

		//glGenTextures(1, &m_textureID);
		//glBindTexture(GL_TEXTURE_2D, m_textureID);

		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		////glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
		//glBindTexture(GL_TEXTURE_2D, 0);

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * m_colors.size(), &m_colors[0], GL_DYNAMIC_DRAW);
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

void Mesh::inverseColor()
{
	std::vector<bool> mask(m_vertics.size(),false);
	for (int i = 0, cnt = m_modifyVerticsIndex.size(); i < cnt; ++i){
		mask[m_modifyVerticsIndex[i]] = true;
	}

	for (int i = 0, cnt = m_colors.size(); i < cnt; ++i){
		if (mask[i]){
			m_displayColors[i] = m_colors[i];
		}
		else{
			m_displayColors[i] = Vec4(1.0, 0.0, 0.0, 0.0);	// 此处指定了， 否则需要存储颜色值
		}
	}
}

// ##恢复原始颜色值
void Mesh::resetColor()
{
	m_displayColors.resize(m_colors.size());
	std::copy(m_colors.begin(), m_colors.end(), m_displayColors.begin());

	m_modifyVerticsIndex.clear();
}

void Mesh::deleteVertics()
{
	m_verticsBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_normalsBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_texturesBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_colorsBak.resize(m_vertics.size() - m_modifyVerticsIndex.size());
	m_indicsBak.clear();

	std::vector<bool> mask(m_vertics.size(), false);
	for (int i = 0, cnt = m_modifyVerticsIndex.size(); i < cnt; ++i){
		mask[m_modifyVerticsIndex[i]] = true;
	}

	int icnt = 0;
	for (int i = 0, cnt = m_vertics.size(); i < cnt; ++i){
		if (!mask[i]){
			m_verticsBak[icnt] = m_vertics[i];
			m_normalsBak[icnt] = m_normals[i];
			m_texturesBak[icnt] = m_textures[i];
			m_colorsBak[icnt] = m_colors[i];
			icnt++;
		}
	}

	// 修改三角索引
	for (int i = 0, cnt = m_indics.size() / 3; i < cnt; i += 3){
		// 如果一个三角形 顶点都没有被标记, 则保留
		if (!mask[m_indics[i + 0]] && !mask[m_indics[i+1]] && !mask[m_indics[i+2]])
		{
			m_indicsBak.push_back(i + 0);
			m_indicsBak.push_back(i + 1);
			m_indicsBak.push_back(i + 2);
		}
	}
	
	flashBuffer(); 
	resetColor();
}

void Mesh::resetVertics()
{

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

	calculateAABB(m_vertics);

	resetColor();
}

void Mesh::flashBuffer()
{
	//glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * m_verticsBak.size(), &m_verticsBak[0]);

	if (m_normalsBak.size() > 0){
		//glGenBuffers(1, &m_normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		glBufferSubData(GL_ARRAY_BUFFER,0, sizeof(Vec3) * m_normalsBak.size(), &m_normalsBak[0]);
	}

	if (m_colorsBak.size() > 0){

		//glGenBuffers(1, &m_colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * m_colorsBak.size(), &m_colorsBak[0]);
	}

	if (m_indicsBak.size() > 0){

		//glGenBuffers(1, &m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_indicsBak.size(), &m_indicsBak[0]);
	}

	if (m_texturesBak.size() > 0){

		//glGenBuffers(1, &m_textureBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_textureBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec2) * m_texturesBak.size(), &m_texturesBak[0]);

	}

	calculateAABB(m_vertics);
}