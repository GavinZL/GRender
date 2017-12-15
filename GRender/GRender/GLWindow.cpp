#include "GL/glew.h"

#include "GLWindow.h"
#include "GRender.h"
#include <QPainter>
#include <QWheelEvent>

#include <iostream>

GLWindow::GLWindow(QWidget* parent/* = nullptr*/)
	: QOpenGLWidget(parent)
	, m_engine(nullptr)
	, m_updateTimer(nullptr)
	, m_scaleRatio(1.0f)
	, m_cameraSensitivity(0.1f)
	, m_cameraMoveSensitivity(1.f)
	, m_dragging(false)
	, m_midDragging(false)
	, m_isAltPressed(false)
	, m_parent(nullptr)
	, m_isCtrlPressed(false)
	, m_selectNode(nullptr)
	, m_isSelectAssist(false)
	, m_renderTexture(nullptr)
{

	m_parent = (GRender*)parent;

	// ###日了鬼了的Qt设置， 放开这儿的注释 glew恩是弄死绘制不出东西来
	// QSurface 's format
	//QSurfaceFormat format;
	//format.setDepthBufferSize(32);
	//format.setMajorVersion(4);
	//format.setMinorVersion(3);
	//format.setProfile(QSurfaceFormat::CoreProfile);
	//format.setOption(QSurfaceFormat::DebugContext);
	//this->setFormat(format);

	setFocusPolicy(Qt::StrongFocus);
}

GLWindow::~GLWindow()
{
	// over Engine 
	if (nullptr != m_engine){
		m_engine->end();
	}

	disconnect(this);
}

void GLWindow::initializeGL()
{
	// OpenGL initialize
	m_engine = G::Engine::getInstance();

	// let us put the timer update init to here
	m_updateTimer = new QTimer(this);
	m_updateTimer->setInterval(16); // f = 1 / 16.10e-3 = 60Hz
	connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_updateTimer->start();

	emit sigOpenGLInitialized();
}

void GLWindow::paintGL()
{
	// render engine
	if (nullptr != m_engine){
		m_engine->mainLoop();
	}
}

void GLWindow::resizeGL(int width, int height)
{
	// resize engine
	if (nullptr != m_engine){
		m_engine->resize(width, height);
	}
	if (m_parent->getPicker()){
		m_parent->getPicker()->resize(width, height);
	}
}

void GLWindow::paintEvent(QPaintEvent *e)
{
	this->makeCurrent();
	QPainter painter(this);

	// frame task
	this->paintGL();

	painter.end();
	this->doneCurrent();
}

void GLWindow::keyPressEvent(QKeyEvent *e)
{
	QOpenGLWidget::keyPressEvent(e);
	switch (e->key())
	{
	case Qt::Key_Space:
		m_engine->getRunningScene()->cameraReset();
		break;
	case Qt::Key_Alt:
		m_isAltPressed = true;
		break;
	case Qt::Key_Control:
		m_isCtrlPressed = true;
		break;

		// test render texture 
	case Qt::Key_F1:
	{
		// 渲染到纹理
		if (nullptr == m_renderTexture){
			m_renderTexture = G::RenderTexture::create(width(), height());
		}

		m_renderTexture->beginWithClearColor(G::Vec4(1.f,0.f,0.f,1.f));
		m_engine->getRunningScene()->visit(m_engine->getRenderer(), G::Mat4::Identity(), 0);
		m_renderTexture->end();

		m_renderTexture->saveToFile("rt.jpg", G::Image::Format::JPG);
	}
		break;
	default:
		break;
	}
}

void GLWindow::keyReleaseEvent(QKeyEvent *e)
{
	switch (e->key())
	{
	case Qt::Key_Alt:
		m_isAltPressed = false;
		if (m_parent->getPicker()){
			m_parent->getPicker()->setEnable(false);
			m_parent->getPicker()->pickedRectangle(G::NodeFlag::_MESH);
		}
		break;

	case Qt::Key_Delete:
	{
		if (m_parent->getPicker()){
			m_parent->getPicker()->deleteVertics();
		}
	}
	break;
	case Qt::Key_R:
	{
		if (m_parent->getPicker()){
			m_parent->getPicker()->restoreVertices();
		}
	}
	break;
	case Qt::Key_I:
	{
		if (m_parent->getPicker()){
			static bool forward = true;
			m_parent->getPicker()->inverseColor(forward);
			forward = !forward;
		}
	}
	break;
	case Qt::Key_C:
	{
		if (m_parent->getPicker()){
			m_parent->getPicker()->restoreColor();
		}
	}
	break;
	case Qt::Key_Control:
		m_isCtrlPressed = false;
		m_isSelectAssist = false;
		break;
	default:
		break;
	}
}

void GLWindow::mousePressEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
	{
		m_dragging = true;
		m_dragStartPoint = e->pos();

		if (m_isCtrlPressed && !m_isSelectAssist){
			rayPickNode(m_dragStartPoint.x(), m_dragStartPoint.y());
			m_isSelectAssist = true;
		}
	}
		break;

	case Qt::MidButton:
	{
		m_midDragging = true;
		m_dragStartPoint = e->pos();
	}
		break;

	default:
		break;
	}
}

void GLWindow::mouseReleaseEvent(QMouseEvent *e)
{
	switch (e->button())
	{
	case Qt::LeftButton:
	{
		m_dragging = false;
	}
		break;

	case Qt::MidButton:
	{
		m_midDragging = false;
	}
		break;

	default:
		break;
	}
}

void GLWindow::mouseMoveEvent(QMouseEvent *e)
{
	e->accept();
	//@@ 是否为选择状态
	if (m_isAltPressed && m_dragging){
		if (m_parent->getPicker()){
			m_parent->getPicker()->setRectangle(G::Vec2(m_dragStartPoint.x(), m_dragStartPoint.y()),
				G::Vec2(e->pos().x(), e->pos().y()));
		}
	}
	else{
		if (m_dragging){
			// ##rotation
			m_dragEndPoint = e->pos();

			float dx = static_cast<float>(m_cameraSensitivity)*
				(static_cast<float>(m_dragEndPoint.x()) - static_cast<float>(m_dragStartPoint.x()));
			float dy = static_cast<float>(-m_cameraSensitivity) *
				(static_cast<float>(m_dragEndPoint.y()) - static_cast<float>(m_dragStartPoint.y()));

			// 旋转选中目标
			if (m_isCtrlPressed && m_selectNode){
				G::Vec3 rot( dy, -dx, 0);
				m_selectNode->setRotationLocal(rot);
			}
			else{
				// 旋转相机
				m_engine->getRunningScene()->setCameraPitch(-dx);
				m_engine->getRunningScene()->setCameraYaw(dy);
			}
			m_dragStartPoint = m_dragEndPoint;
		}
		else if (m_midDragging){
			// translate
			m_dragEndPoint = e->pos();
			float dx = static_cast<float>(-m_cameraMoveSensitivity)*
				(static_cast<float>(m_dragEndPoint.x()) - static_cast<float>(m_dragStartPoint.x()));
			float dy = static_cast<float>(m_cameraMoveSensitivity)*
				(static_cast<float>(m_dragEndPoint.y()) - static_cast<float>(m_dragStartPoint.y()));

			// 移动选中目标
			if (m_isCtrlPressed && m_selectNode){
				G::Vec3 offset(-dx, -dy, 0);
				offset *= 0.5;
				m_selectNode->setPositionLocal(offset);
			}
			else{
				// 移动相机
				m_engine->getRunningScene()->setCameraTrans(G::Vec3(-dx, dy, 0));
			}
			m_dragStartPoint = m_dragEndPoint;
		}
	}
}

void GLWindow::wheelEvent(QWheelEvent *e)
{
	e->accept();
	float sr = e->delta() > 0 ? 1.2 : 0.8;

	m_scaleRatio = sr;

	if (!qFuzzyCompare(m_scaleRatio, 0.0f)){
		m_engine->getRunningScene()->setCameraScale(m_scaleRatio);
		m_scaleRatio = 1.0f;
	}
}

void GLWindow::mouseDoubleClickEvent(QMouseEvent *e)
{

}

//  [12/13/2017 Administrator]


void GLWindow::rayPickNode(int x, int y)
{
	G::Vec3 dir = m_engine->getRunningScene()->getDefaultCamera()->getMousePointToWorld(x, y, width(), height());
	G::Vec3 pos = m_engine->getRunningScene()->getDefaultCamera()->getPosition();

	G::Ray ray(pos, dir);

	std::vector<G::Node*> meshNodes;
	m_engine->getRunningScene()->visit(m_engine->getRunningScene(), &ray, meshNodes, G::_MESH);

	// 如果之前选中的节点，在重新选中时要恢复之前的颜色
	if (m_selectNode != nullptr){
		m_selectNode->setBoundingBoxColor(G::Color3(1.0, 1.0, 1.0));
	}

	if (meshNodes.size() > 0){
		m_selectNode = meshNodes[0];
		m_selectNode->setBoundingBoxColor(G::Color3(1.0, 0.0, 0.0));
	}
}
