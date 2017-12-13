/************************************************************************/
/* 
* ʰȡ��������
* ɾ���� ������ ���õȱ༭���� һ��Ҳ�ڴ��ദ����
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

	// ���ô����С
	void resize(int w, int h);
	virtual void draw(Renderer* renderer, const Mat4& transform, unsigned int flags) override;

	/**@@����*/
	// ���þ��ο�
	void setRectangle(const Vec2& origin, const Vec2& dest);

	// ��Ⱦ���ο�
	void renderRectangle(const Mat4& transform, unsigned int flags);

	// **����
	void setEnable(bool b){ m_isRectDirty = b; }

	/**@@ʰȡ*/
	void pickedRectangle(unsigned int nodeFlag);

	/**�޸�mesh����ɫֵ*/
	void modifyMeshColor(std::vector<Node*>& nodes, std::vector<unsigned int>& indics);


	/**ɾ������*/
	void deleteVertics();

	/**�ָ�����*/
	void restoreVertices();

	/**�ָ���ɫ*/
	void restoreColor();

	/**��תѡ��*/
	void inverseColor(bool forward);

protected:

	// ##window size, [0]: width, [1]: height
	int m_winSize[2]; 

	// ##screen coord points
	int m_oriPos[2];
	int m_curPos[2];

	// ##���ο򶥵�
	bool m_isRectDirty;
	float m_rectangleV[12];
	CustomCommand m_customCommand;

	// ## ����ʰȡ�Ľڵ�
	std::vector<Node*> m_pickedNodes;
};


NAMESPACE_END

#endif