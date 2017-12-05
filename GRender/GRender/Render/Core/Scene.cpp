

#include "Scene.h"
#include "Camera.h"
#include "Engine.h"
#include "Renderer.h"

#include "../Comm/Utils.h"

USING_NAMESPACE_G


Scene* Scene::create()
{
	Scene *_scene = new (std::nothrow) Scene();
	if (_scene){
		_scene->init();
		return _scene;
	}
	return nullptr;
}

Scene::Scene()
	: m_pitch(0.0f)
	, m_yaw(0.0f)
{
	m_defaultCamera = Camera::create();
	//m_defaultCamera->setPosition(Vec3(100, 0, 150));
	//m_defaultCamera->setRotation(Vec3(0, 1.57f, 0));
	this->addChild(m_defaultCamera);
}

Scene::~Scene()
{

}

void Scene::resize(int width, int height)
{
	if (m_defaultCamera != nullptr){
		m_defaultCamera->resize(width, height);
	}
}

bool Scene::init()
{

	//setRotation(Vec3(0, 0, DEG_TO_RAD(-45)));

	return true;
}

std::string Scene::getDescription() const
{
	return G::format(">>>>> Scene | Tag = %d ", m_tag);
}

const std::vector<Camera*>& Scene::getCameras() const
{
	return m_cameras;
}

const std::vector<Light*>& Scene::getLigths() const
{
	return m_lights;
}

void Scene::setCameraScale(float sl)
{
	m_defaultCamera->setScale(sl);
	//Vec3 eyeToCenter = m_defaultCamera->getCenter() - m_defaultCamera->getPosition();
	//eyeToCenter *= sl;

	//m_defaultCamera->setPosition(m_defaultCamera->getCenter() - eyeToCenter);
}

void Scene::render(Renderer* renderer)
{
	//...
	auto engine = Engine::getInstance();


	Camera* defaultCamera = nullptr;
	const auto& transform = this->getNodeToParentTransform();

	// ##渲染除了默认相机之外的其他相机
	for (const auto& camera : m_cameras){
		
		Camera::g_visitingCamera = camera;
		if (Camera::g_visitingCamera == m_defaultCamera){
			continue;
		}

		engine->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		engine->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, Camera::g_visitingCamera->getViewProjectionMatrix());

		// ##遍历子树
		this->visit(renderer, transform, 0);

		// ##此处为何要进行渲染操作???
		renderer->render();

		engine->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	}

	// ##默认渲染相机
	if (m_defaultCamera)
	{
		Camera::g_visitingCamera = m_defaultCamera;
		engine->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		engine->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, Camera::g_visitingCamera->getViewProjectionMatrix());
		
		// ##遍历子树
		this->visit(renderer, transform, 0);

		// ##此处为何要进行渲染操作???
		renderer->render();

		engine->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	}

	Camera::g_visitingCamera = nullptr;
}


void Scene::updateCamera(float deta)
{
	//if (m_pitch > EPSILON && m_yaw > EPSILON){

		m_defaultCamera->setPose(m_pitch, m_yaw);
		m_pitch = 0;
		m_yaw = 0;
	//}
}

void Scene::setCameraTrans(const Vec3& v)
{ 
	float factor = m_defaultCamera->getCameraToCenter().norm();

	Vec3 mv = v;
	if (factor < 100){
		mv *= 0.01;
	}
	else
	{
		mv *= factor * 0.001;
	}

	m_defaultCamera->translateLocal(mv);
}

void Scene::cameraReset()
{
	m_defaultCamera->reset();
}