/************************************************************************/
/* 
* µÆ¹â»ùÀà£¬subclass : directional , point, spot
*/
/************************************************************************/

#ifndef  __LIGHT_H__
#define  __LIGHT_H__

#include "../Comm/Macors.h"

#include "Node.h"

NAMESPACE_BEGIN

enum LightType
{
	DIRECTION = 0,
	POINT,
	SPOT,
	AMBIENT
};

enum LightFlag
{
	LIGHT0 = 1,
	LIGHT1 = 1 << 1,
	LIGHT2 = 1 << 2,
	LIGHT3 = 1 << 3,
	LIGHT4 = 1 << 4,
	LIGHT5 = 1 << 5,
	LIGHT6 = 1 << 6,
	LIGHT7 = 1 << 7
};

//  [12/7/2017 Administrator]
class Light : public Node
{
public:
	Light();
	virtual ~Light();

	// get light type
	virtual LightType getLightType() const = 0;

	// setter or getter intensity
	inline float getIntensity() const{ return m_intensity; }
	inline void setIntensity(float i) { m_intensity = i; }

	// setter or getter flag
	inline LightFlag getLightFlag() const { return m_lightFlag; }
	inline void setLightFlag(LightFlag flag) { m_lightFlag = flag; }

	// getter or setter enabled
	inline void setEnable(bool enable){ m_enabled = enable; }
	inline bool isEnable() const { return m_enabled; }


	//override
	virtual void onEnter() override;
	virtual void onExit() override;

protected:
	float m_intensity;
	LightFlag m_lightFlag;
	bool m_enabled;

};

//  [12/7/2017 Administrator]
class DirectionLight : public Light
{
public:
	// crate direction light
	// direction 
	// color
	static DirectionLight* create(const Vec3& direction, const Color3& color);

	// get light type
	virtual LightType getLightType() const override { return LightType::DIRECTION; }

	//setter or getter direction
	void setDirection(const Vec3& dir);

	Vec3 getDirection();

	Vec3 getDirectionInWorld();

protected:
	DirectionLight();
	virtual ~DirectionLight();

};


//  [12/7/2017 Administrator]

class PointLight : public Light
{
public:
	static PointLight* create(const Vec3& position, const Color3& color, float range);

	virtual LightType getLightType() const override { return LightType::POINT; }

	float getRange()  const { return m_range; }
	void setRange(float ran){ m_range = ran; }

protected:
	PointLight();
	virtual ~PointLight();

protected:

	float m_range;
};


//  [12/7/2017 Administrator]

class SpotLight : public Light
{
public:

	static SpotLight* create(const Vec3& directoin, const Vec3& position, const Color3& color,
		float innerAngle, float outerAngle, float range);

	//get light type
	virtual LightType getLightType() const override { return LightType::SPOT; }


	//setter or getter direction
	void setDirection(const Vec3& dir);

	Vec3 getDirection();

	Vec3 getDirectionInWorld();

	/**
	* Sets the range of point or spot light.
	*
	* @param range The range of point or spot light.
	*/
	void setRange(float range) { m_range = range; }

	/**
	* Returns the range of point or spot light.
	*
	* @return The range of the point or spot light.
	*/
	float getRange() const { return m_range; }
	/**
	* Sets the inner angle of a spot light (in radians).
	*
	* @param angle The angle of spot light (in radians).
	*/
	void setInnerAngle(float angle);

	/**
	* Returns the inner angle the spot light (in radians).
	*/
	float getInnerAngle() const { return m_innerAngle; }

	/** get cos innerAngle */
	float getCosInnerAngle() const { return m_cosInnerAngle; }

	/**
	* Sets the outer angle of a spot light (in radians).
	*
	* @param outerAngle The angle of spot light (in radians).
	*/
	void setOuterAngle(float angle);

	/**
	* Returns the outer angle of the spot light (in radians).
	*/
	float getOuterAngle() const { return m_outerAngle; }

	/** get cos outAngle */
	float getCosOuterAngle() const { return m_cosOuterAngle; }

protected:
	SpotLight();
	virtual ~SpotLight();

protected:
	float m_range;
	float m_innerAngle;
	float m_cosInnerAngle;
	float m_outerAngle;
	float m_cosOuterAngle;
};

//  [12/7/2017 Administrator]

class AmbientLight : public Light
{
public:
	static AmbientLight* create(const Color3& color);

	//get light type
	virtual LightType getLightType() const override { return LightType::AMBIENT; }

protected:
	AmbientLight();
	virtual ~AmbientLight();
};

NAMESPACE_END


#endif