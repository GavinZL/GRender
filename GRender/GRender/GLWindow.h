/************************************************************************/
/* opengl 渲染窗口 (Qt + OpenGL集成类) 
* 
*/
/************************************************************************/

#ifndef  __GLWINDOW_H__
#define  __GLWINDOW_H__

#include <QOpenGLWidget>
#include <QTimer>
#include <QPoint>

#include "Render/GEngine.h"

class GLWindow : public QOpenGLWidget
{
	Q_OBJECT
public:
	GLWindow(QWidget* parent = nullptr);
	virtual ~GLWindow();

public:
	void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);
	virtual void paintEvent(QPaintEvent *e);

	virtual void keyPressEvent(QKeyEvent *);
	virtual void keyReleaseEvent(QKeyEvent *);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

public:
	/**
	 * Get Engine pointer
	 */
	G::Engine* getEngine() { return m_engine; }

signals:
	// opengl 窗口初始化完成
	void sigOpenGLInitialized();

private:
	/** Engine */
	G::Engine * m_engine;

	/** update timer */
	QTimer *m_updateTimer;

	/**control camera*/
	float m_scaleRatio;
	float m_cameraSensitivity;
	float m_cameraMoveSensitivity;

	QPoint m_dragStartPoint;
	QPoint m_dragEndPoint;

	bool m_dragging;
	bool m_midDragging;
};


#endif