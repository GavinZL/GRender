/************************************************************************/
/* �̳�Node�࣬ ��Ҫʵ����άģ�ͽڵ� 
*  Ŀǰ��������� һ��meshnode һ��mesh�Ľṹ
*/
/************************************************************************/

#ifndef  __MESHNODE_H__
#define  __MESHNODE_H__

#include "../Comm/Macors.h"

#include "Node.h"
#include "CustomCommand.h"

NAMESPACE_BEGIN

class Mesh;
class Renderer;

class MeshNode  : public Node
{
public:
	MeshNode();
	MeshNode(const std::string& filePath);
	~MeshNode();

	/**initialize mesh with file path*/
	bool initMesh(const std::string& modelPath);

	bool initMesh(const std::vector<Vec3>& position,
		const std::vector<Vec3>& normals,
		const std::vector<Vec4>& colors,
		const std::vector<Vec2>& texs,
		const std::vector<unsigned int>& indics);

	/**return mesh*/
	inline Mesh* getMesh(){ return m_mesh; }

	virtual void draw(Renderer* renderer, const Mat4& transform, unsigned int flags) override;

public:

	/**ѡ��shader����*/
	void genGLProgramState(bool useLight);

	/** light mask getter & setter, light works only when _lightmask & light's flag is true, 
	* default value of _lightmask is 0xffff 
	*/
	void setLightMask(unsigned int mask) { m_lightMask = mask; }
	unsigned int getLightMask() const { return m_lightMask; }

protected:
	/**update aabb box */
	bool m_aabbDirty;

	unsigned int                 m_lightMask;
	bool                         m_shaderUsingLight; // is current shader using light ?

	/** default one mesh*/
	Mesh* m_mesh;
};

NAMESPACE_END

#endif