/************************************************************************/
/* 
* 相机类
*/
/************************************************************************/

#ifndef  __CAMERA_H__
#define  __CAMERA_H__

#include "../Comm/Macors.h"
#include "../Comm/Math.h"

#include "Node.h"

NAMESPACE_BEGIN

class Scene;

class Camera : public Node
{
public:
	// ##相机类型
	enum class Type
	{
		PERSPECTIVE = 1,
		ORTHOGRAPHIC = 2
	};

public:

	/**
	* Creates a perspective camera.
	*
	* @param fieldOfView(Degress) The field of view for the perspective camera (normally in the range of 40-60 degrees).
	* @param aspectRatio The aspect ratio of the camera (normally the width of the viewport divided by the height of the viewport).
	* @param nearPlane The near plane distance.
	* @param farPlane The far plane distance.
	*/
	static Camera* createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

	/**
	* Creates an orthographic camera.
	*
	* @param zoomX The zoom factor along the X-axis of the orthographic projection (the width of the ortho projection).
	* @param zoomY The zoom factor along the Y-axis of the orthographic projection (the height of the ortho projection).
	* @param aspectRatio The aspect ratio of the orthographic projection.
	* @param nearPlane The near plane distance.
	* @param farPlane The far plane distance.
	*/
	static Camera* createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane);

	/** create default camera, the camera type depends on Director::getProjection */
	static Camera* create();

	/**
	* Gets the type of camera.
	*/
	Camera::Type getType() const { return m_type; }


	/**
	* Creates a view matrix based on the specified input parameters.
	*
	* @param eyePosition The eye position.
	* @param targetPosition The target's center position.
	* @param up The up vector.
	* @param dst A matrix to store the result in.
	*/
	virtual void lookAt(const Vec3& target, const Vec3& up);


	/**
	* Gets the camera's projection matrix.
	*
	*/
	const Mat4& getProjectionMatrix() const;

	/**
	* Gets the camera's view matrix.
	*
	*/
	const Mat4& getViewMatrix();


	/**get view projection matrix.
	 *
	 */
	const Mat4& getViewProjectionMatrix();

	/**
	* Convert the specified point of viewport from screenspace coordinate into the worldspace coordinate.
	*/
	void unproject(const Vec2& viewport, Vec3* src, Vec3* dst) const;


	// ##return current visiting camera
	static const Camera* getVisitingCamera() { return g_visitingCamera; }

	//override
	virtual void onEnter() override;
	virtual void onExit() override;

	void resize(int w, int h);
	void reset();

	// return lookat center
	inline Vec3 getCenter(){ return m_center; }
	
	// return up direction
	inline Vec3 getUp(){ return m_up;  }

	inline Vec3 getCameraToCenter(){ return m_cameraToCenter; }

	// ##设置旋转量
	//void setRotation(const Vec3& rot) override;
	void setPosition(const Vec3& position) override;
	void setScale(float scale);

	void setPose(float pitch, float yaw);

	void setPitch(float pitchRad);
	void setYaw(float yawRad);

	// 平移
	void translateLocal(const Vec3& local);
	void translateWorld(const Vec3& world);

protected:
	Camera();
	virtual ~Camera();

	bool init();

	void setScene(Scene* scene);

	bool initPerspective(float fieldOfView, float aspectRatio, float near, float far);
	bool initOrthographic(float zoomx, float zoomy, float near, float far);

protected:

	// ##投影矩阵
	Mat4 m_projection;

	mutable Mat4 m_view;
	mutable Mat4 m_viewInv;
	mutable Mat4 m_viewProjection;

	// ##相机类型
	Camera::Type m_type;

	// perspective
	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;

	// orthographic
	float m_zoom[2];

	// ## up && center
	Vec3 m_up;
	Vec3 m_center;
	Vec3 m_cameraToCenter;	// direction

	Scene* m_scene; //Scene camera belongs to

	// ##是否需要更新视图投影矩阵
	mutable bool m_viewMatrixDirty;
	mutable bool m_viewProjectionDirty;

public:
	// ##用于全局访问
	static Camera* g_visitingCamera;
};

NAMESPACE_END
#endif