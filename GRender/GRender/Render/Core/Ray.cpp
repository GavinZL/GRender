#include "Ray.h"

USING_NAMESPACE_G


Ray::Ray()
: m_direction(Vec3(0,0,1))
, m_origin(Vec3::Zero())
{

}

Ray::Ray(const Ray& ray)
{
	set(ray.m_origin, ray.m_direction);
}

Ray::Ray(const Vec3& origin, const Vec3& direction)
{
	set(origin, direction);
}

/**Destructor*/
Ray::~Ray()
{

}

/**Check its intersect the aabb box*/
bool Ray::intersects(const AABB& aabb)
{
	Vec3 ptOnPlane;
	Vec3 min = aabb._min;
	Vec3 max = aabb._max;

	const Vec3& origin = m_origin;
	const Vec3& dir = m_direction;

	float t;

	if (dir[0] != 0.f){
		if (dir[0] > 0.f){
			t = (min[0] - origin[0]) / dir[0];
		}
		else
		{
			t = (max[0] - origin[0]) / dir[0];
		}

		if (t > 0.f){
			ptOnPlane = origin + dir * t;
			if (min[1] < ptOnPlane[1] && ptOnPlane[1] < max[1]
				&& min[2] < ptOnPlane[2] && ptOnPlane[2] < max[2]){
				return true;
			}
		}
	}

	if (dir[1] != 0.f){
		if (dir[1] > 0.f){
			t = (min[1] - origin[1]) / dir[1];
		}
		else{
			t = (max[1] - origin[1]) / dir[1];
		}

		if (t > 0.f){
			ptOnPlane = origin + dir * t;
			if (min[2] < ptOnPlane[2] && ptOnPlane[2] < max[2]
				&& min[0] < ptOnPlane[0] && ptOnPlane[0] < max[0]){
				return true;
			}
		}
	}

	if (dir[2] != 0.f){
		if (dir[2] > 0.f){
			t = (min[2] - origin[2]) / dir[2];
		}
		else{
			t = (max[2] - origin[2]) / dir[2];
		}

		if (t > 0.f){
			ptOnPlane = origin + dir * t;
			if (min[0] < ptOnPlane[0] && ptOnPlane[0] < max[0]
				&& min[1] < ptOnPlane[1] && ptOnPlane[1] < max[1]){
				return true;
			}
		}
	}

	return false;
}

/**Set the ray 's params*/
void Ray::set(const Vec3& origin, const Vec3& direction)
{
	m_direction = direction;
	m_origin = origin;
}