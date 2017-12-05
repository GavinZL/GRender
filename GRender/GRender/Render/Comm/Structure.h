/************************************************************************/
/* ##此文件主要借助Eigen矩阵库来定义内部使用的 vec2，vec3, vec4, mat3, mat4 
*  ## quaternion 等
*/
/************************************************************************/

#ifndef  __STRUCTURE_H__
#define  __STRUCTURE_H__

// ##包含Eigen文件
#include <Eigen/Eigen>
#include "Macors.h"

NAMESPACE_BEGIN

/**
 * 二维数组
 */
typedef Eigen::Vector2f Vec2;

/**
 *三维数组
 */
typedef Eigen::Vector3f Vec3;


/**
 *四维数组
 */
typedef Eigen::Vector4f Vec4;


/**
 *四元素
 */
typedef Eigen::Quaternion<float> Quaternion;

/**
 *3x3 矩阵
 */
typedef Eigen::Matrix3f Mat3;

/**
 *4x4 矩阵
 */
typedef Eigen::Matrix4f Mat4;


/**
 * 颜色
 */
typedef Vec3 Color3;
typedef Vec4 Color4;

/**
 * 顶点
 */
typedef Vec3 Point;


typedef Eigen::AngleAxisf Angle_Axis;


NAMESPACE_END

#endif