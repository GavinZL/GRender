/************************************************************************/
/* 
* 绘制基本元素类
*/
/************************************************************************/

#ifndef  __DRAWNODE_H__
#define  __DRAWNODE_H__


#include "../Comm/Macors.h"
#include "Node.h"
#include "CustomCommand.h"


#include <vector>

NAMESPACE_BEGIN


class DrawNode : public Node
{
public:
	enum Type
	{
		POINT = 0,  // single point
		POINTS,		// many points
		LINE,		// single line
		LINE_LOOP,	// line loop
		AXIS,		// axis
		BOX,		// line box
		CUBE,		// solid box
		SPHERE,		// sphere
		SCANNER,	// scanner
		MAX
	};

public:

	DrawNode();
	virtual ~DrawNode();

	/**
	* ##初始化需要绘制类型
	* ##params 传入初始化参数
	*/
	bool init(Type _type, void* params);

	/**
	* draw functions
	*/
	void drawPoint(const Vec3& point, const float ptSize, const Color4& color);
	void drawPoints(const std::vector<Vec3>& pts, const float ptSize, const Color4& color);

	void drawLine(const Vec3& origin, const Vec3& destination, const float lineSize, const Color4& color);
	void drawLineLoop(const std::vector<Vec3>& vertices, const float lineSize, const Color4& color);

	void drawAxis(const float length, const float lineSize);
	void drawBox(const float x_duration, const float y_duration, const float z_duration, const float lineSize);
	
	void drawCube(const float x_duration, const float y_duration, const float z_duration);
	void drawSphere(const float raduis);

	void drawScanner();

	/**renderer callback*/
	void onDraw(const Mat4& transform, unsigned int flags);

	/**override draw func*/
	virtual void draw(Renderer* renderer, const Mat4& transform, unsigned int flags) override;

protected:

	/**标记*/
	bool m_dirty;
	bool m_initialized;

	/**Current type*/
	GLenum m_type;

	/**绘制Buffer*/
	GLuint m_vbo;
	GLuint m_vertexBuffer;
	GLuint m_colorBuffer;
	std::vector<Vec3> m_vertices;
	std::vector<Vec4> m_colors;

	// ...
	float m_pointSize; 
	float m_lineWidth;

	CustomCommand m_customCommand;


	/**Test buffer id*/
	GLuint _positionID;
	GLuint _colorID;

	GLuint _programId;

};


NAMESPACE_END


#endif