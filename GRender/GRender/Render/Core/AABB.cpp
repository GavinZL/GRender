#include "AABB.h"

USING_NAMESPACE_G


AABB::AABB()
{
	reset();
}

AABB::AABB(const Vec3& min, const Vec3& max)
{
	set(min, max);
}

AABB::AABB(const AABB& box)
{
	set(box._min, box._max);
}

Vec3 AABB::getCenter()
{
	Vec3 center;
	center[0] = 0.5f*(_min[0] + _max[0]);
	center[1] = 0.5f*(_min[1] + _max[1]);
	center[2] = 0.5f*(_min[2] + _max[2]);

	return center;
}

void AABB::getCorners(Vec3 *dst) const
{
	assert(dst);

	// Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
	// Left-top-front.
	dst[0] = Vec3(_min[0], _max[1], _max[2]);
	// Left-bottom-front.
	dst[1] = Vec3(_min[0], _min[1], _max[2]);
	// Right-bottom-front.
	dst[2] = Vec3(_max[0], _min[1], _max[2]);
	// Right-top-front.
	dst[3] = Vec3(_max[0], _max[1], _max[2]);

	// Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
	// Right-top-back.
	dst[4] = Vec3(_max[0], _max[1], _min[2]);
	// Right-bottom-back.
	dst[5] = Vec3(_max[0], _min[1], _min[2]);
	// Left-bottom-back.
	dst[6] = Vec3(_min[0], _min[1], _min[2]);
	// Left-top-back.
	dst[7] = Vec3(_min[0], _max[1], _min[2]);
}

bool AABB::intersects(const AABB& aabb) const
{
	return ((_min[0] >= aabb._min[0] && _min[0] <= aabb._max[0]) || (aabb._min[0] >= _min[0] && aabb._min[0] <= _max[0])) 
		&& ((_min[1] >= aabb._min[1] && _min[1] <= aabb._max[1]) || (aabb._min[1] >= _min[1] && aabb._min[1] <= _max[1])) 
		&& ((_min[2] >= aabb._min[2] && _min[2] <= aabb._max[2]) || (aabb._min[2] >= _min[2] && aabb._min[2] <= _max[2]));
}

bool AABB::containPoint(const Vec3& point) const
{
	if (point[0] < _min[0]) return false;
	if (point[1] < _min[1]) return false;
	if (point[2] < _min[2]) return false;
	if (point[0] > _max[0]) return false;
	if (point[1] > _max[1]) return false;
	if (point[2] > _max[2]) return false;
	return true;
}

void AABB::merge(const AABB& box)
{
	// Calculate the new minimum point.
	_min[0] = std::min(_min[0], box._min[0]);
	_min[1] = std::min(_min[1], box._min[1]);
	_min[2] = std::min(_min[2], box._min[2]);

	// Calculate the new maximum point.
	_max[0] = std::max(_max[0], box._max[0]);
	_max[1] = std::max(_max[1], box._max[1]);
	_max[2] = std::max(_max[2], box._max[2]);
}

void AABB::set(const Vec3& min, const Vec3& max)
{
	this->_min = min;
	this->_max = max;
}

void AABB::reset()
{
	_min = Vec3(99999.0f, 99999.0f, 99999.0f);
	_max = Vec3(-99999.0f, -99999.0f, -99999.0f);
}

bool AABB::isEmpty() const
{
	return _min[0] > _max[0] || _min[1] > _max[1] || _min[2] > _max[2];
}

void AABB::updateMinMax(const Vec3* point, unsigned int num)
{
	for (unsigned int i = 0; i < num; i++)
	{
		// Leftmost point.
		if (point[i][0] < _min[0])
			_min[0] = point[i][0];

		// Lowest point.
		if (point[i][1] < _min[1])
			_min[1] = point[i][1];

		// Farthest point.
		if (point[i][2] < _min[2])
			_min[2] = point[i][2];

		// Rightmost point.
		if (point[i][0] > _max[0])
			_max[0] = point[i][0];

		// Highest point.
		if (point[i][1] > _max[1])
			_max[1] = point[i][1];

		// Nearest point.
		if (point[i][2] > _max[2])
			_max[2] = point[i][2];
	}
}

void AABB::updateMinMax(const std::vector<Vec3> & point)
{
	for (unsigned int i = 0, num = point.size(); i < num; i++)
	{
		// Leftmost point.
		if (point[i][0] < _min[0])
			_min[0] = point[i][0];

		// Lowest point.
		if (point[i][1] < _min[1])
			_min[1] = point[i][1];

		// Farthest point.
		if (point[i][2] < _min[2])
			_min[2] = point[i][2];

		// Rightmost point.
		if (point[i][0] > _max[0])
			_max[0] = point[i][0];

		// Highest point.
		if (point[i][1] > _max[1])
			_max[1] = point[i][1];

		// Nearest point.
		if (point[i][2] > _max[2])
			_max[2] = point[i][2];
	}
}

void AABB::transform(const Mat4& mat)
{
	Vec3 corners[8];
	// Near face, specified counter-clockwise
	// Left-top-front.
	corners[0] = Vec3(_min[0], _max[1], _max[2]);
	// Left-bottom-front.
	corners[1] = Vec3(_min[0], _min[1], _max[2]);
	// Right-bottom-front.
	corners[2] = Vec3(_max[0], _min[1], _max[2]);
	// Right-top-front.
	corners[3] = Vec3(_max[0], _max[1], _max[2]);

	// Far face, specified clockwise
	// Right-top-back.
	corners[4] = Vec3(_max[0], _max[1], _min[2]);
	// Right-bottom-back.
	corners[5] = Vec3(_max[0], _min[1], _min[2]);
	// Left-bottom-back.
	corners[6] = Vec3(_min[0], _min[1], _min[2]);
	// Left-top-back.
	corners[7] = Vec3(_min[0], _max[1], _min[2]);

	// Transform the corners, recalculate the min and max points along the way.
	for (int i = 0; i < 8; i++){
		//mat.transformPoint(&corners[i]);
		Vec4 v = Vec4(corners[i][0], corners[i][1], corners[i][2], 1.0);
		Vec4 vv = mat * v;
		corners[i][0] = vv[0];
		corners[i][1] = vv[1];
		corners[i][2] = vv[2];
	}

	reset();

	updateMinMax(corners, 8);
}

void AABB::transformOffset(const Vec3& offset)
{
	Vec3 corners[8];
	// Near face, specified counter-clockwise
	// Left-top-front.
	corners[0] = Vec3(_min[0], _max[1], _max[2]);
	// Left-bottom-front.
	corners[1] = Vec3(_min[0], _min[1], _max[2]);
	// Right-bottom-front.
	corners[2] = Vec3(_max[0], _min[1], _max[2]);
	// Right-top-front.
	corners[3] = Vec3(_max[0], _max[1], _max[2]);

	// Far face, specified clockwise
	// Right-top-back.
	corners[4] = Vec3(_max[0], _max[1], _min[2]);
	// Right-bottom-back.
	corners[5] = Vec3(_max[0], _min[1], _min[2]);
	// Left-bottom-back.
	corners[6] = Vec3(_min[0], _min[1], _min[2]);
	// Left-top-back.
	corners[7] = Vec3(_min[0], _max[1], _min[2]);

	// Transform the corners, recalculate the min and max points along the way.
	for (int i = 0; i < 8; i++){
		//mat.transformPoint(&corners[i]);
		corners[i] += offset;
	}

	reset();

	updateMinMax(corners, 8);
}