/************************************************************************/
/* 数学方面函数 
 *
 */
/************************************************************************/

#ifndef  __MATHFUNCTION_H__
#define  __MATHFUNCTION_H__

#include "Macors.h"
#include "Math.h"

NAMESPACE_BEGIN

/**
* @@angle-axis  转换到 3x3矩阵
* @@ angle : rad angle
* @@ a1 : rotation axis
*/
extern G::Mat3 matrixFromAxisAngle(float angle, const G::Vec3& a1);


/**
* Creates a perspective matrix.
*
* @param fieldOfView(Degree) The field of view for the perspective camera (normally in the range of 40-60 degrees).
* @param aspectRatio The aspect ratio of the camera (normally the width of the viewport divided by the height of the viewport).
* @param nearPlane The near plane distance.
* @param farPlane The far plane distance.
*/
extern G::Mat4 createPerspectiveMatrix(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

/**
* Creates an orthographic Matrix.
*
* @param width The zoom factor along the X-axis of the orthographic projection (the width of the ortho projection).
* @param height The zoom factor along the Y-axis of the orthographic projection (the height of the ortho projection).
* @param nearPlane The near plane distance.
* @param farPlane The far plane distance.
*/
extern G::Mat4 createOrthographicMatrix(float width, float height, float nearPlane, float farPlane);


/**
* Creates a view matrix based on the specified input parameters.
*
* @param eyePosition The eye position.
* @param targetPosition The target's center position.
* @param upDir The up vector.
* @return A matrix to store the result in.
*/
extern G::Mat4 createLookAtMatrix(const Vec3& eyePosition, const Vec3& targetPosition, const Vec3& upDir);

extern G::Mat3 createLookAtMatrix3(const Vec3& eyePosition, const Vec3& targetPosition, const Vec3& upDir);




NAMESPACE_END

#endif