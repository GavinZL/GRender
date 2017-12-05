/************************************************************************/
/* 
* AABB bounding box
*/
/************************************************************************/

#ifndef  __AABB_H__
#define  __AABB_H__


#include "../Comm/Macors.h"
#include "../Comm/Math.h"
#include "../Comm/Utils.h"

NAMESPACE_BEGIN

class AABB
{
public:

	/**constructor*/
	AABB();
	AABB(const Vec3& min, const Vec3& max);
	AABB(const AABB& box);

	/**get AABB box center*/
	Vec3 getCenter();

	/* Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
	* verts[0] : left top front
	* verts[1] : left bottom front
	* verts[2] : right bottom front
	* verts[3] : right top front
	*
	* Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
	* verts[4] : right top back
	* verts[5] : right bottom back
	* verts[6] : left bottom back
	* verts[7] : left top back
	*/
	void getCorners(Vec3* dst) const;


	/**
	* Tests whether this bounding box intersects the specified bounding object.
	*/
	bool intersects(const AABB& aabb) const;

	/**
	* check whether the point is in.
	*/
	bool containPoint(const Vec3& point) const;


	/**
	* Sets this bounding box to the smallest bounding box
	* that contains both this bounding object and the specified bounding box.
	*/
	void merge(const AABB& box);

	/**
	* Sets this bounding box to the specified values.
	*/
	void set(const Vec3& min, const Vec3& max);


	/**
	* reset min and max value.
	*/
	void reset();

	bool isEmpty() const;

	/**
	* update the _min and _max from the given point.
	*/
	void updateMinMax(const Vec3* point, unsigned int num);
	void updateMinMax(const std::vector<Vec3> & points);


	/**
	* Transforms the bounding box by the given transformation matrix.
	*/
	void transform(const Mat4& mat);


public:
	Vec3 _min;
	Vec3 _max;
};


NAMESPACE_END

#endif