/************************************************************************/
/* 
* 拾取场景数据
* 删除， 撤销， 重置等编辑工作 一并也在此类处理了
*/
/************************************************************************/
#ifndef  __PICKER_H__
#define  __PICKER_H__

#include "../Comm/Macors.h"
#include "Node.h"
#include "CustomCommand.h"

NAMESPACE_BEGIN


class Picker : public Node
{
public:
	static Picker* create(int w, int h);

public:
	Picker();
	virtual ~Picker();

	// 设置窗体大小
	void resize(int w, int h);
	virtual void draw(Renderer* renderer, const Mat4& transform, unsigned int flags) override;

	/**@@绘制*/
	// 设置矩形框
	void setRectangle(const Vec2& origin, const Vec2& dest);

	// 渲染矩形框
	void renderRectangle(const Mat4& transform, unsigned int flags);

	// **运行
	void setEnable(bool b){ m_isRectDirty = b; }

	/**@@拾取*/
	void pickedRectangle(unsigned int nodeFlag);

	/**修改mesh的颜色值*/
	void modifyMeshColor(std::vector<Node*>& nodes, std::vector<unsigned int>& indics);


	/**删除顶点*/
	void deleteVertics();

	/**恢复顶点*/
	void restoreVertices();

	/**恢复颜色*/
	void restoreColor();

	/**反转选择*/
	void inverseColor(bool forward);

protected:

	// ##window size, [0]: width, [1]: height
	int m_winSize[2]; 

	// ##screen coord points
	int m_oriPos[2];
	int m_curPos[2];

	// ##矩形框顶点
	bool m_isRectDirty;
	float m_rectangleV[12];
	CustomCommand m_customCommand;

	// ## 保留拾取的节点
	std::vector<Node*> m_pickedNodes;
};


NAMESPACE_END

#endif