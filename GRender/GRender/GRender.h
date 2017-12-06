#ifndef GRENDER_H
#define GRENDER_H

#include <QtWidgets/QMainWindow>
#include "ui_grender.h"
#include <QDragEnterEvent>
#include <QDropEvent>

class GLWindow;

class GRender : public QMainWindow
{
	Q_OBJECT

public:
	GRender(QWidget *parent = 0);
	~GRender();

	// drag files in
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

private:
	Ui::GRenderClass ui;

	// main opengl window
	GLWindow *m_glWindow;
};

#endif // GRENDER_H
