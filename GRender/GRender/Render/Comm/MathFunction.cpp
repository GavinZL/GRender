/************************************************************************/
/* 数学方面函数 
 *
 */
/************************************************************************/
#include "Macors.h"
#include "Math.h"

#include "Utils.h"

//#include <QMatrix4x4>
//#include <QDebug>
#include <iostream>


NAMESPACE_BEGIN

/**
 * @@angle-axis  转换到 3x3矩阵
 * @@ angle : rad angle
 * @@ a1 : rotation axis
 */
G::Mat3 matrixFromAxisAngle(float angle, const G::Vec3& a1)
{
	double c = std::cosf(angle);
	double s = std::sinf(angle);
	double t = 1.0 - c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(a1.x*a1.x + a1.y*a1.y + a1.z*a1.z);
	// if (magnitude==0) throw error;
	// a1.x /= magnitude;
	// a1.y /= magnitude;
	// a1.z /= magnitude;
	Mat3 m;
	m(0, 0) = c + a1(0) * a1(0) * t;
	m(1, 1) = c + a1(1) * a1(1) * t;
	m(2, 2) = c + a1(2) * a1(2) * t;

	double tmp1 = a1(0) * a1(1) * t;
	double tmp2 = a1(2) * s;
	m(1, 0) = tmp1 + tmp2;
	m(0, 1) = tmp1 - tmp2;
	tmp1 = a1(0) * a1(2) * t;
	tmp2 = a1(1) * s;
	m(2, 0) = tmp1 - tmp2;
	m(0, 2) = tmp1 + tmp2;
	tmp1 = a1(1) * a1(2) * t;
	tmp2 = a1(0) * s;
	m(2, 1) = tmp1 + tmp2;
	m(1, 2) = tmp1 - tmp2;

	return m;
}



G::Mat4 createPerspectiveMatrix(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	if (nearPlane == farPlane){
		G::log(">>>>> createPerspectiveMatrix : nearPlane == farPlane !");
		return G::Mat4::Zero();
	}

	if (aspectRatio < EPSILON){
		G::log(">>>>> createPerspectiveMatrix : aspectRatio == 0 !");
		return G::Mat4::Zero();
	}

	float f_n = 1.0f / (farPlane - nearPlane);

	float theta = DEG_TO_RAD(fieldOfView) * 0.5f;

	if (fabs(fmod(theta, PIOVER2)) < EPSILON2){
		G::log(">>>>> Invalid field of view value(%f) causes attemped calculation tan(%f), \
			   			   			   			which is undefined ", fieldOfView, theta);
		return G::Mat4::Zero();
	}

	float divisor = tan(theta);

	if (divisor < EPSILON){
		G::log(">>>>> createPerspectiveMatrix : fieldOfView is invalid !");
		return G::Mat4::Zero();
	}

	float factor = 1.0f / divisor;

	// ## identity
	G::Mat4 _projection;
	_projection.setZero();

	_projection.data()[0] = (1.0f / aspectRatio) * factor;
	_projection.data()[5] = factor;
	_projection.data()[10] = (-(farPlane + nearPlane)) * f_n;
	_projection.data()[11] = -1.0f;
	_projection.data()[14] = -2.0f * farPlane * nearPlane * f_n;

	//_projection(0, 0) = (1.0f / aspectRatio) * factor;
	//_projection(1, 1) = factor;
	//_projection(2, 2) = (-(farPlane + nearPlane)) * f_n;
	//_projection(2, 3) = -2.0f * farPlane * nearPlane * f_n;
	//_projection(3, 2) = -1.0f;

	//std::cout << " ------------------------------- " << std::endl;
	//std::cout << _projection << std::endl;
	//std::cout << " ------------------------------- " << std::endl;
	//QMatrix4x4 mmt;
	//mmt.perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
	//qDebug() << mmt;


	return _projection;
}

G::Mat4 createOrthographicMatrix(float width, float height, float nearPlane, float farPlane)
{
	float hw = width * 0.5f;
	float hh = height * 0.5f;

	float left = -hw, right = hw;
	float bottom = -hh, top = hh;

	if (left == right || bottom == top || nearPlane == farPlane){
		G::log(">>>>> createOrthographicMatrix : invalid params !");
		return G::Mat4::Zero();
	}

	// ##matrix
	G::Mat4 _projection;
	_projection.setZero();

	_projection.data()[0] = 2 / (right - left);
	_projection.data()[5] = 2 / (top - bottom);
	_projection.data()[10] = 2 / (farPlane - nearPlane);

	_projection.data()[12] = (left + right) / (left - right);
	_projection.data()[13] = (top + bottom) / (bottom - top);
	_projection.data()[14] = (nearPlane + farPlane) / (nearPlane - farPlane);
	_projection.data()[15] = 1.0f;

	//_projection(0, 0) = 2 / (right - left);
	//_projection(1, 1) = 2 / (top - bottom);
	//_projection(2, 2) = 2 / (farPlane - nearPlane);

	//_projection(3, 0) = (left + right) / (left - right);
	//_projection(3, 1) = (top + bottom) / (bottom - top);
	//_projection(3, 2) = (nearPlane + farPlane) / (nearPlane - farPlane);
	//_projection(3, 3) = 1.0f;

	return _projection;
}

G::Mat4 createLookAtMatrix(const Vec3& eyePosition, const Vec3& targetPosition, const Vec3& upDir)
{
	// ##camera params
	Vec3 eye(eyePosition);
	Vec3 target(targetPosition);
	Vec3 up(upDir);

	up.normalize();

	// ##camera coordinates
	Vec3 zaxis = eye - target;
	zaxis.normalize();

	Vec3 xaxis;
	xaxis = up.cross(zaxis);
	xaxis.normalize();

	Vec3 yaxis;
	yaxis = zaxis.cross(xaxis);
	yaxis.normalize();

	G::Mat4 _view;
	_view.setZero();

	_view.data()[0] = xaxis(0);
	_view.data()[1] = yaxis(0);
	_view.data()[2] = zaxis(0);
	_view.data()[3] = 0.0f;

	_view.data()[4] = xaxis(1);
	_view.data()[5] = yaxis(1);
	_view.data()[6] = zaxis(1);
	_view.data()[7] = 0.0f;

	_view.data()[8] = xaxis(2);
	_view.data()[9] = yaxis(2);
	_view.data()[10] = zaxis(2);
	_view.data()[11] = 0.0f;

	_view.data()[12] = -xaxis.dot(eye);
	_view.data()[13] = -yaxis.dot(eye);
	_view.data()[14] = -zaxis.dot(eye);
	_view.data()[15] = 1.0f;

	//_view(0, 0) = xaxis(0);
	//_view(0, 1) = yaxis(0);
	//_view(0, 2) = zaxis(0);
	//_view(0, 3) = 0.0f;

	//_view(1, 0) = xaxis(1);
	//_view(1, 1) = yaxis(1);
	//_view(1, 2) = zaxis(1);
	//_view(1, 3) = 0.0f;

	//_view(2, 0) = xaxis(2);
	//_view(2, 1) = yaxis(2);
	//_view(2, 2) = zaxis(2);
	//_view(2, 3) = 0.0f;

	//_view(3, 0) = -xaxis.dot(eye);
	//_view(3, 1) = -yaxis.dot(eye);
	//_view(3, 2) = -zaxis.dot(eye);
	//_view(3, 3) = 1.0f;

	//QMatrix4x4 mml;
	//mml.lookAt(QVector3D(eye[0], eye[1], eye[2]), QVector3D(target[0], target[1], target[2]),
	//	QVector3D(up[0],up[1],up[2]));

	//std::cout << " View ------------------------------- " << std::endl;
	//std::cout << _view << std::endl;
	//std::cout << " ------------------------------- " << std::endl;

	//qDebug() << mml;


	return _view;
}


G::Mat3 createLookAtMatrix3(const Vec3& eyePosition, const Vec3& targetPosition, const Vec3& upDir)
{

	//Vec3 f = (targetPosition - eyePosition).normalized();
	//Vec3 u = upDir.normalized();
	//Vec3 s = f.cross(u).normalized();
	//u = s.cross(f);

	//Mat3 res;
	//res << s.x(), s.y(), s.z(),
	//	u.x(), u.y(), u.z(),
	//	-f.x(), -f.y(), -f.z();

	//return res;

	// ##camera params
	Vec3 eye(eyePosition);
	Vec3 target(targetPosition);
	Vec3 up(upDir);

	up.normalize();

	// ##camera coordinates
	Vec3 zaxis = eye - target;
	zaxis.normalize();

	Vec3 xaxis;
	xaxis = up.cross(zaxis);
	xaxis.normalize();

	Vec3 yaxis;
	yaxis = zaxis.cross(xaxis);
	yaxis.normalize();

	G::Mat3 _view;
	_view.setZero();

	_view.data()[0] = xaxis(0);
	_view.data()[1] = yaxis(0);
	_view.data()[2] = zaxis(0);

	_view.data()[3] = xaxis(1);
	_view.data()[4] = yaxis(1);
	_view.data()[5] = zaxis(1);

	_view.data()[6] = xaxis(2);
	_view.data()[7] = yaxis(2);
	_view.data()[8] = zaxis(2);

	return _view;
}


NAMESPACE_END