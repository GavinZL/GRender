/************************************************************************/
/* �̳�Node�࣬ ��Ҫʵ����άģ�ͽڵ� 
*  Ŀǰ��������� һ��meshnode һ��mesh�Ľṹ
*/
/************************************************************************/

#ifndef  __MESHNODE_H__
#define  __MESHNODE_H__

#include "../Comm/Macors.h"

#include "Node.h"
#include "Light.h"
#include "CustomCommand.h"

NAMESPACE_BEGIN

class Mesh;
class Renderer;

class DrawNode;

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
		const std::vector<unsigned int>& indics,
		const std::string& texturePath);

	/**return mesh*/
	inline Mesh* getMesh(){ return m_mesh; }

	virtual void draw(Renderer* renderer, const Mat4& transform, unsigned int flags) override;

	virtual void setBoundingBoxColor(const Color3& color) override;
public:

	/**ѡ��shader����*/
	void genGLProgramState(bool useLight);

	/** light mask getter & setter, light works only when _lightmask & light's flag is true, 
	* default value of _lightmask is 0xffff 
	* ���õ�ǰ �ڵ㣬 ������һյ ���Ǽ�յ�ĵƹ�[Ĭ�������еƹⶼ����]
	*/
	void setLightMask(unsigned int mask) { m_lightMk = mask; }
	unsigned int getLightMask() const { return m_lightMk; }

	/**����boungding box*/
	virtual void hideBoundingBox(bool b) override;

	/**����boundingbox ״̬*/
	virtual bool boundingBoxVisible() override{ return m_aabbVisiable; }

	virtual void updateBoundingBox() override;

protected:
	/**update aabb box */
	bool m_aabbVisiable;

	unsigned int                 m_lightMk;
	bool                         m_shaderUsingLight; // is current shader using light ?

	/** default one mesh*/
	Mesh* m_mesh;

	/** ��ʾaabb*/
	DrawNode* m_boxNode;
};

NAMESPACE_END

#endif