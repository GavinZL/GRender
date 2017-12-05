#include "GRender.h"
#include "GLWindow.h"


#include "Render/GEngine.h"

GRender::GRender(QWidget *parent)
	: QMainWindow(parent)
	, m_glWindow(nullptr)
{
	ui.setupUi(this);

	resize(1280, 960);

	// opengl window
	m_glWindow = new (std::nothrow) GLWindow(this);
	this->setCentralWidget(m_glWindow);

	/**#等待opengl窗口初始化完成*/
	connect(m_glWindow, &GLWindow::sigOpenGLInitialized, [this]()
	{
		G::Scene *scene = G::Scene::create();

		G::DrawNode *drawNode = new G::DrawNode();
		drawNode->init(G::DrawNode::AXIS, nullptr);
		drawNode->drawAxis(1.0f, 1.0f);
		drawNode->setPosition(G::Vec3(0, 0, 0));
		drawNode->setScale(G::Vec3(100, 100, 100));

		scene->addChild(drawNode);

		m_glWindow->getEngine()->runWithScene(scene);
	});

}

GRender::~GRender()
{

}
