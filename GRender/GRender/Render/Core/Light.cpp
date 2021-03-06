#include "Light.h"
#include "Scene.h"
#include "Camera.h"

USING_NAMESPACE_G;

Light::Light()
	: m_intensity(1.0)
	, m_enabled(true)
	, m_lightFlag((unsigned int)LIGHT0)
{
	m_nodeFlagMask = _LIGHT;
}

Light::~Light()
{

}

void Light::onEnter()
{
	auto scene = getScene();
	if (scene){
		auto &lights = scene->m_lights;
		auto iter = std::find(lights.begin(), lights.end(), this);
		if (iter == lights.end()){
			lights.push_back(this);
		}
	}

	Node::onEnter();
}

void Light::onExit()
{
	auto scene = getScene();
	if (scene){
		auto &lights = scene->m_lights;
		auto iter = std::find(lights.begin(), lights.end(), this);
		if (iter != lights.end()){
			lights.erase(iter);
		}
	}
	Node::onExit();
}

// [12/7/2017 Administrator]

DirectionLight::DirectionLight()
	:Light()
{

}

DirectionLight::~DirectionLight()
{

}

DirectionLight* DirectionLight::create(const Vec3& direction, const Color3& color)
{
	auto light = new (std::nothrow) DirectionLight();
	light->setDirection(direction);
	light->setDisplayColor(color);

	return light;
}

void DirectionLight::setDirection(const Vec3& dir)
{
	setRotation(dir);
}

Vec3 DirectionLight::getDirection()
{
	//// 此处默认为跟随相机的位置看向目标物的方向
	//if (Camera::g_visitingCamera){

	//	Vec3 dir = Camera::g_visitingCamera->getCameraToCenter();
	//	dir.normalize();

	//	return dir;
	//}

	//return Vec3::Zero();

	Mat4 m = getNodeToParentTransform();
	Mat3 m3 = m.block(0, 0, 3, 3);
	Angle_Axis ax(m3);

	return ax.axis() * ax.angle();
}

Vec3 DirectionLight::getDirectionInWorld()
{
	//// 此处默认为跟随相机的位置看向目标物的方向
	//if (Camera::g_visitingCamera){

	//	Vec3 dir = Camera::g_visitingCamera->getCameraToCenter();
	//	dir.normalize();

	//	return dir;
	//}

	//return Vec3::Zero();

	Mat4 m = getNodeToWorldTransform();
	Mat3 m3 = m.block(0, 0, 3, 3);
	Angle_Axis ax(m3);

	return ax.axis() * ax.angle();
}

//  [12/7/2017 Administrator]

PointLight::PointLight()
	:Light()
{

}

PointLight::~PointLight()
{

}

PointLight* PointLight::create(const Vec3& position, const Color3& color, float range)
{
	auto light = new (std::nothrow) PointLight;
	light->setPosition(position);
	light->setDisplayColor(color);

	light->setRange(range);

	return light;
}

//  [12/7/2017 Administrator]

SpotLight::SpotLight()
	:Light()
{

}

SpotLight::~SpotLight()
{

}

SpotLight* SpotLight::create(const Vec3& directoin, const Vec3& position, const Color3& color,
	float innerAngle, float outerAngle, float range)
{
	auto light = new (std::nothrow) SpotLight();
	light->setPosition(position);
	light->setDisplayColor(color);
	light->setInnerAngle(innerAngle);
	light->setOuterAngle(outerAngle);
	light->m_range = range;

	return light;
}

void SpotLight::setDirection(const Vec3 &dir)
{
	setRotation(dir);
}

Vec3 SpotLight::getDirection()
{
	Mat4 m = getNodeToParentTransform();
	Mat3 m3 = m.block(0, 0, 3, 3);
	Angle_Axis ax(m3);

	return ax.axis() * ax.angle();
}

Vec3 SpotLight::getDirectionInWorld()
{
	Mat4 m = getNodeToWorldTransform();
	Mat3 m3 = m.block(0, 0, 3, 3);
	Angle_Axis ax(m3);

	return ax.axis() * ax.angle();
}

void SpotLight::setInnerAngle(float angle)
{
	m_innerAngle = angle;
	m_cosInnerAngle = cosf(angle);
}

void SpotLight::setOuterAngle(float angle)
{
	m_outerAngle = angle;
	m_cosOuterAngle = cosf(angle);
}

//  [12/7/2017 Administrator]

AmbientLight::AmbientLight()
	:Light()
{

}

AmbientLight::~AmbientLight()
{

}

AmbientLight* AmbientLight::create(const Color3& color)
{
	auto light = new (std::nothrow) AmbientLight;
	light->setDisplayColor(color);

	return light;
}