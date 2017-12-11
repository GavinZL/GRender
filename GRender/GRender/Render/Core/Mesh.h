/************************************************************************/
/* 
* 主要用于存储meshnode的数据[顶点，索引，属性]
*/
/************************************************************************/

#ifndef  __MESH_H__
#define  __MESH_H__

#include "../Comm/Macors.h"
#include "../Comm/Math.h"
#include "../GDefine.h"
#include "Ref.h"
#include "MeshCommand.h"

NAMESPACE_BEGIN

class Texture2D;
class AABB;
class GLProgramState;

class Mesh : public Ref
{
public:
	/**create mesh */
	static Mesh* create(const std::vector<Vec3>& position,
		const std::vector<Vec3>& normals,
		const std::vector<Vec4>& colors,
		const std::vector<Vec2>& texs,
		const std::vector<unsigned int>& indics,
		const std::string& texturePath = "");

	/**calculate aabb box*/
	void calculateAABB(const std::vector<Vec3>& vertics);

	/**get  aabb box*/
	inline AABB* getAABB() const { return m_aabb; }

	inline GLuint getVertexBuffer() { return m_vertexBuffer; }
	inline GLuint getNormalBuffer(){ return m_normalBuffer; }
	inline GLuint getColorBuffer() { return m_colorBuffer; }
	inline GLuint getIndexBuffer(){ return m_indexBuffer; }
	inline GLuint getTextureBuffer(){ return m_textureBuffer; }

	inline Texture2D* getTexture2D(){ return m_texture; }
	void setTexture2D(Texture2D* tex);

	inline void setGLProgramState(GLProgramState* glprogramState){ m_glProgramState = glprogramState; }
	inline GLProgramState* getGLProgramState(){ return m_glProgramState; }

	inline void setMeshCommand(const MeshCommand& meshCommand){ m_meshCommand = meshCommand; }
	inline MeshCommand* getMeshCommand(){ return &m_meshCommand; }

	inline void setName(const std::string &name){ m_name = name; }
	inline std::string getName(){ return m_name; }

	inline void setTransparent(bool b){ m_transparent = b; };
	inline bool isTransparent(){ return m_transparent; }

	inline void setVisible(bool b){ m_visible = b; }
	inline bool isVisible(){ return m_visible; }

public:
	Mesh();
	~Mesh();

	bool init(const std::vector<Vec3>& position,
		const std::vector<Vec3>& normals,
		const std::vector<Vec4>& colors,
		const std::vector<Vec2>& texs,
		const std::vector<unsigned int>& indics,
		const std::string& texturePath);

	inline bool hasNormal(){ return m_normals.size() > 0; }
	inline bool hasTexture(){ return m_textures.size() > 0; }
	inline bool hasColor(){ return m_colors.size() > 0; }
	inline bool hasIndics(){ return m_indics.size() > 0; }


	inline int getIndexCount(){ return m_indics.size(); }
	inline int getVertexCount(){ return m_vertics.size(); }
	inline GLuint getPrimitive(){ return m_primitive; }

protected:

	/**draw texture or  draw color*/
	bool m_isTextureMesh;

	/**mesh visible */
	bool m_visible; 

	/**mesh transparent*/
	bool m_transparent;
	
	/**Mesh name*/
	std::string m_name;

	/**gl program state*/
	GLProgramState *m_glProgramState;

	/**render command*/
	MeshCommand m_meshCommand;

	/**box*/
	AABB *m_aabb;

	/**texture*/
	Texture2D* m_texture;

	/**OpenGL buffers*/
	GLuint m_vertexBuffer;
	GLuint m_normalBuffer;
	GLuint m_colorBuffer;
	GLuint m_indexBuffer;
	GLuint m_textureBuffer;

	std::vector<Vec3> m_vertics;
	std::vector<Vec4> m_colors;
	std::vector<Vec3> m_normals;
	std::vector<Vec2> m_textures;
	std::vector<unsigned int> m_indics;


	// 
	GLuint m_primitive;

	public:
	// test
	GLuint m_textureID;
};


NAMESPACE_END


#endif