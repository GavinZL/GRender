#include "GRender.h"
#include "GLWindow.h"
#include "Render/GEngine.h"
#include "Render/Core/MeshNode.h"

#include <QMimeData>
#include <QStringList>
#include <QList>

GRender::GRender(QWidget *parent)
	: QMainWindow(parent)
	, m_glWindow(nullptr)
{
	ui.setupUi(this);

	resize(1280, 960);

	// opengl window
	m_glWindow = new (std::nothrow) GLWindow(this);
	this->setCentralWidget(m_glWindow);

	this->setAcceptDrops(true);
	
	/**#等待opengl窗口初始化完成*/
	connect(m_glWindow, &GLWindow::sigOpenGLInitialized, [this]()
	{
		G::Scene *scene = G::Scene::create();

		G::DrawNode *drawNode = new G::DrawNode();
		drawNode->init(G::DrawNode::AXIS, nullptr);
		drawNode->drawAxis(1.0f, 2.0f);
		drawNode->setPosition(G::Vec3(0, 0, 0));
		drawNode->setScale(G::Vec3(10, 10, 10));

		scene->addChild(drawNode);


		G::DrawNode *drawBox = new (std::nothrow) G::DrawNode();
		drawBox->init(G::DrawNode::CUBE, nullptr);
		drawBox->drawCube(G::Vec3(-1, -1, -1), G::Vec3(1, 1, 1));
		scene->addChild(drawBox);

		m_glWindow->getEngine()->runWithScene(scene);
	});
}

GRender::~GRender()
{

}

void GRender::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();
}

void GRender::dropEvent(QDropEvent *event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls())
	{
		QStringList pathList;
		QList<QUrl> urlList = mimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			pathList.append(urlList.at(i).toLocalFile());
		}

		if (pathList.size() > 0)
		{
			//MainModel::instance()->modelFileName = pathList.at(0);

			//event->acceptProposedAction();

			QString filePath = pathList.at(0);

			G::MeshNode *node = new (std::nothrow) G::MeshNode(filePath.toStdString());
			if (node){
				m_glWindow->getEngine()->getRunningScene()->addChild(node);
			}
		}
	}
}
