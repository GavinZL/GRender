

#include "Scene.h"
#include "Camera.h"
#include "Engine.h"
#include "Renderer.h"
#include "Light.h"

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
	, m_defaultDirLight(nullptr)
{
	m_nodeFlagMask = _SCENE;

	// default camera
	m_defaultCamera = Camera::create();
	this->addChild(m_defaultCamera);

	// default ambient light
	AmbientLight* ambLight = AmbientLight::create(Color3(1, 1, 1));
	ambLight->setIntensity(0.3);
	this->addChild(ambLight);

	// default directional light
	m_defaultDirLight = DirectionLight::create(Vec3(0, 1, 0), Color3(0.6, 0.6, 0.6));
	this->addChild(m_defaultDirLight);

	//PointLight* pointLight = PointLight::create(Vec3(10,10, 0), Color3(0, 0, 1), 0.3);
	//this->addChild(pointLight);

	// ##初始化刷新direction light的方向
	updateCamera(0);
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
	m_defaultCamera->setPose(m_pitch, m_yaw);
	m_pitch = 0;
	m_yaw = 0;

	// ##实时更新默认平行光的位置
	Vec3 dir = m_defaultCamera->getCameraToCenter();
	dir.normalize();
	m_defaultDirLight->setDirection(dir);
}

Mat3  Scene::getCameraLookAtMatrix(const Vec3& targetPos , const Vec3& up)
{
	return m_defaultCamera->getLookAtMatrix(targetPos, up);
}

void Scene::setCameraTrans(const Vec3& v)
{ 
	float factor = m_defaultCamera->getCameraToCenter().norm();

	Vec3 mv = v;
	/*if (factor < 100){
		mv *= 0.01;
	}
	else
	{
		mv *= factor * 0.001;
	}
	*/
	mv *= factor * 0.001;
	m_defaultCamera->translateLocal(mv);
}

void Scene::cameraReset()
{
	m_defaultCamera->reset();
}