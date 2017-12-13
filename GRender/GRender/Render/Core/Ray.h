/************************************************************************/
/* 
* 用于射线拾取
*/
/************************************************************************/

#ifndef  __RAY_HH__
#define  __RAY_HH__

#include "../Comm/Macors.h"

#include "AABB.h"

NAMESPACE_BEGIN

class Ray
{
public:

	/**Constructor*/
	Ray();
	Ray(const Ray& ray);
	Ray(const Vec3& origin, const Vec3& direction);

	/**Destructor*/
	virtual ~Ray();

	/**Check its intersect the aabb box*/
	bool intersects(const AABB& aabb);

	/**Set the ray 's params*/
	void set(const Vec3& origin, const Vec3& direction);

protected:
	Vec3 m_origin;
	Vec3 m_direction;

};

NAMESPACE_END


#endif