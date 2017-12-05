/************************************************************************/
/* ##包含数学相关的文件
 * 
 */
/************************************************************************/

#ifndef  __MATH_H__
#define  __MATH_H__

#include <numeric>

/**
 * 包含结构
 */
#include "Structure.h"

//#include "MathFunction.h"

NAMESPACE_BEGIN

/**
 * PI
 */
#define PI 3.1415926535898;

/**
 * 角度转弧度
 */
#define  DEG_TO_RAD(x) ((x) * 0.0174532925)

/**
 * 弧度转角度
 */
#define  RAD_TO_DEG(x) ((x) * 57.29577951)

/**
 * 最小误差
 */
#define  EPSILON std::numeric_limits<double>::epsilon()

#define  PIOVER2 1.57079632679489661923f
#define  EPSILON2 0.000001f


inline float clampf(float value, float min_inclusive, float max_inclusive)
{
	if (min_inclusive > max_inclusive) {
		std::swap(min_inclusive, max_inclusive);
	}
	return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
}

NAMESPACE_END

#endif