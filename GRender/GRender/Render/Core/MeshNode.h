/************************************************************************/
/* 继承Node类， 主要实现三维模型节点 
*  目前处理情况， 一个meshnode 一个mesh的结构
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

	/**选择shader类型*/
	void genGLProgramState(bool useLight);

	/** light mask getter & setter, light works only when _lightmask & light's flag is true, 
	* default value of _lightmask is 0xffff 
	* 设置当前 节点， 接收那一盏 或那几盏的灯光[默认是所有灯光都接收]
	*/
	void setLightMask(unsigned int mask) { m_lightMk = mask; }
	unsigned int getLightMask() const { return m_lightMk; }

	/**隐藏boungding box*/
	virtual void hideBoundingBox(bool b) override;

	/**返回boundingbox 状态*/
	virtual bool boundingBoxVisible() override{ return m_aabbVisiable; }

	virtual void updateBoundingBox() override;

protected:
	/**update aabb box */
	bool m_aabbVisiable;

	unsigned int                 m_lightMk;
	bool                         m_shaderUsingLight; // is current shader using light ?

	/** default one mesh*/
	Mesh* m_mesh;

	/** 显示aabb*/
	DrawNode* m_boxNode;
};

NAMESPACE_END

#endif