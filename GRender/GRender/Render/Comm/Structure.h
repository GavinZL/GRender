/************************************************************************/
/* ##���ļ���Ҫ����Eigen������������ڲ�ʹ�õ� vec2��vec3, vec4, mat3, mat4 
*  ## quaternion ��
*/
/************************************************************************/

#ifndef  __STRUCTURE_H__
#define  __STRUCTURE_H__

// ##����Eigen�ļ�
#include <Eigen/Eigen>
#include "Macors.h"

NAMESPACE_BEGIN

/**
 * ��ά����
 */
typedef Eigen::Vector2f Vec2;

/**
 *��ά����
 */
typedef Eigen::Vector3f Vec3;


/**
 *��ά����
 */
typedef Eigen::Vector4f Vec4;


/**
 *��Ԫ��
 */
typedef Eigen::Quaternion<float> Quaternion;

/**
 *3x3 ����
 */
typedef Eigen::Matrix3f Mat3;

/**
 *4x4 ����
 */
typedef Eigen::Matrix4f Mat4;


/**
 * ��ɫ
 */
typedef Vec3 Color3;
typedef Vec4 Color4;

/**
 * ����
 */
typedef Vec3 Point;


typedef Eigen::AngleAxisf Angle_Axis;


NAMESPACE_END

#endif