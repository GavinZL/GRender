#ifndef GRENDER_H
#define GRENDER_H

#include <QtWidgets/QMainWindow>
#include "ui_grender.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QResizeEvent>

#include "Render/GEngine.h"

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

	// get picker
	inline G::Picker* getPicker(){ return m_picker; }

private:
	Ui::GRenderClass ui;

	// main opengl window
	GLWindow *m_glWindow;

	//Ê°È¡½áµã
	G::Picker* m_picker;

private:
	QToolBar *m_toolBar;
	QAction *m_testButton;

	void initToolBar();
};

#endif // GRENDER_H
