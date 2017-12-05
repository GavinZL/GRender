#include "Camera.h"

#include "Engine.h"

#include "../Comm/Utils.h"
#include "../Comm/Math.h"
#include "../Comm/MathFunction.h"
#include "Scene.h"

#include <iostream>

USING_NAMESPACE_G

Camera* Camera::g_visitingCamera = nullptr;

Camera::Camera()
:m_viewProjectionDirty(true)
, m_scene(nullptr)
, m_type(Camera::Type::PERSPECTIVE)
, m_center(Vec3::Zero())
, m_up(Vec3(0,1,0))
{
	m_view.setIdentity();
	m_viewInv.setIdentity();
	m_viewProjection.setIdentity();
}

Camera::~Camera()
{

}

Camera* Camera::create()
{
	Camera* _camera = new (std::nothrow) Camera;
	_camera->init();
	return _camera;
}

void Camera::resize(int w, int h)
{
	this->init();
}

void Camera::reset()
{
	m_view.setIdentity();
	m_viewInv.setIdentity();
	m_viewProjection.setIdentity();

	Vec3 eye(0.0, 0.0, 150.0);
	Vec3 center(Vec3::Zero());
	Vec3 up(0.0, 1.0, 0.0);

	m_center = center;
	m_up = up;

	setPosition(eye);
	lookAt(center, up);
}

Camera* Camera::createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	auto _camera = new (std::nothrow) Camera;
	if (_camera){
		_camera->initPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);
		return _camera;
	}
	return nullptr;
}

Camera* Camera::createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
{
	auto _camera = new (std::nothrow) Camera;
	if (_camera){
		_camera->initOrthographic(zoomX, zoomY, nearPlane, farPlane);
		return _camera;
	}

	return nullptr;
}

bool Camera::init()
{
	auto size = Engine::getInstance()->getWinSize();
	auto projection = Engine::getInstance()->getProjection();

	switch (projection)
	{
	case G::Engine::Projection::_2D:
	{
		initOrthographic(size(0), size(1), -1024, 1024);
		setPosition(Vec3::Zero());
		setRotation(Vec3::Zero());
		break;
	}
	case G::Engine::Projection::_3D:
	{
		initPerspective(60.0f, (float)size(0) / size(1), 0.1f, 1000000);
		Vec3 eye(0.0, 0.0, 150.0);
		Vec3 center(Vec3::Zero());
		Vec3 up(0.0, 1.0, 0.0);

		m_center = center;
		m_up = up;

		setPosition(eye);
		lookAt(center, up);

		break;
	}
	default:
	{
		G::log(">>>>> Camera::init() : unrecognized projection !");
		break;
	}
	}

	return true;
}

void Camera::onEnter()
{
	if (nullptr == m_scene){
		auto scene = getScene();

		if (scene){
			setScene(scene);
		}
	}

	Node::onEnter();
}

void Camera::onExit()
{
	setScene(nullptr);
	Node::onExit();
}

void Camera::setScene(Scene* scene)
{
	if (m_scene != scene){
		// delete old scene
		if (m_scene){
			auto& cameras = m_scene->m_cameras;
			auto it = std::find(cameras.begin(), cameras.end(), this);
			if (it != cameras.end()){
				cameras.erase(it);
			}
			m_scene = nullptr;
		}

		// set new scene
		if (scene){
			m_scene = scene;

			auto &camears = m_scene->m_cameras;
			auto it = std::find(camears.begin(), camears.end(), this);
			if (it == camears.end()){
				camears.push_back(this);
			}
		}
	}
}

bool Camera::initPerspective(float fieldOfView, float aspectRatio, float near, float far)
{
	m_fieldOfView = fieldOfView;
	m_aspectRatio = aspectRatio;
	m_nearPlane = near;
	m_farPlane = far;
	
	m_projection = G::createPerspectiveMatrix(fieldOfView, aspectRatio, near, far);

	m_viewProjectionDirty = true;
	return true;
}

bool Camera::initOrthographic(float zoomx, float zoomy, float near, float far)
{
	m_zoom[0] = zoomx;
	m_zoom[1] = zoomy;
	m_nearPlane = near;
	m_farPlane = far;

	m_projection = G::createOrthographicMatrix(zoomx, zoomy, near, far);

	m_viewProjectionDirty = true;
	return true;
}

void Camera::lookAt(const Vec3& target, const Vec3& up)
{
	G::Mat3 viewM = G::createLookAtMatrix3(this->getPosition(), target, up);
	
	// ##此处去逆，表示绕着target旋转
	G::Mat3 viewInv = viewM.inverse();

	// ##此处getViewMatrix是直接从Camera node中获取的 mv矩阵
	// ##所以此处将 m_view 转化为角度，设置给camera::rotation
	//G::Quaternion quaternion(viewM);
	//float rotx = atan2f(2 * (quaternion.w() * quaternion.x() + quaternion.y() * quaternion.z()),
	//	1 - 2 * (quaternion.x() * quaternion.x() + quaternion.y() * quaternion.y()));
	//float roty = asin(clampf(2 * (quaternion.w() * quaternion.y() - quaternion.z() * quaternion.x()), -1.0f, 1.0f));
	//float rotz = -atan2(2 * (quaternion.w() * quaternion.z() + quaternion.x() * quaternion.y()),
	//	1 - 2 * (quaternion.y() * quaternion.y() + quaternion.z() * quaternion.z()));
	//this->setRotation(Vec3(rotx, roty, rotz));

	Angle_Axis aaxis(viewInv);
	Vec3 ax = aaxis.axis() * aaxis.angle();
	
	this->setRotation(ax);

	m_center = target;
	m_up = up;
}

const Mat4& Camera::getProjectionMatrix() const
{
	return m_projection;
}

const Mat4& Camera::getViewMatrix()
{
	Mat4 viewInv = this->getNodeToWorldTransform();

	if (viewInv != m_viewInv){
		m_viewProjectionDirty = true;
		m_viewInv = viewInv;
		m_view = viewInv.inverse();
	}

	return m_view;
}

const Mat4& Camera::getViewProjectionMatrix()
{
	getViewMatrix();
	if (m_viewProjectionDirty){
		m_viewProjectionDirty = false;

		m_viewProjection = m_projection * m_view;
	}

	return m_viewProjection;
}

void Camera::setPosition(const Vec3& position)
{	
	Node::setPosition(position);	

	m_cameraToCenter = position - m_center;
	m_viewMatrixDirty = true;
	
	lookAt(m_center, m_up);
}

void Camera::setPitch(float pitchRad)
{
	Vec3 vUp = m_up.normalized();
	Vec3 vDi = m_cameraToCenter.normalized();

	Vec3 x_axis = vUp.cross(vDi);
	x_axis.normalize();

	Angle_Axis xaxis(DEG_TO_RAD(pitchRad), x_axis);
	m_up = xaxis * m_up;
	m_cameraToCenter = xaxis * m_cameraToCenter;
	m_position = m_center + m_cameraToCenter;
}

void Camera::setYaw(float yawRad)
{
	Vec3 vUp = m_up.normalized();

	Angle_Axis aaxis = Angle_Axis(DEG_TO_RAD(yawRad), vUp);
	m_up = aaxis * m_up;
	m_cameraToCenter = aaxis * m_cameraToCenter;
	m_position = m_center + m_cameraToCenter;
}


void Camera::setPose(float pitch, float yaw)
{
	setPitch(yaw);
	setYaw(pitch);
	lookAt(m_center, m_up);
}

void  Camera::setScale(float scale)
{
	m_cameraToCenter *= scale;

	setPosition(m_center + m_cameraToCenter);
}


void Camera::translateLocal(const Vec3& local)
{
	Vec3 world;		
	Vec3 up = m_up.normalized();
	Vec3 cc = m_cameraToCenter.normalized();
	if (local[0] != 0.0f){
		Vec3 x_axis = cc.cross(up);
		x_axis.normalize();

		world += x_axis * local[0] ;
	}
	if (local[1] != 0.0f){
		world += up * local[1];
	}
	if (local[2] != 0.0f){
		world += cc * local[2];
	}

	m_center += world;

	m_cameraToCenter = m_position - m_center;

	cc = m_cameraToCenter.normalized();

	Vec3 x = cc.cross(up);
	x.normalize();

	m_up = x.cross(cc);
	m_up.normalize();

	//m_transformDirty = true;
	lookAt(m_center, m_up);	
}

void Camera::translateWorld(const Vec3& world)
{
	m_position += world;
	m_cameraToCenter = m_position - m_center;

	m_transformDirty = true;
}

void Camera::unproject(const Vec2& viewport, Vec3* src, Vec3* dst) const
{

}