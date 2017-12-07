/************************************************************************/
/* scene graph 的root节点 
* 
*/
/************************************************************************/

#ifndef  __SCENE_H__
#define  __SCENE_H__

#include "../Comm/Macors.h"

#include "Node.h"

NAMESPACE_BEGIN

class Camera;
class Light;
class Renderer;

class Scene : public Node
{
public:
	static Scene* create();

	void resize(int width, int height);

	// ## 使用node的addchild方法
	using Node::addChild;

	// ##描述
	virtual std::string getDescription() const override;

	const std::vector<Camera*>& getCameras() const;
	const std::vector<Light*>& getLigths() const;

	inline Camera* getDefaultCamera(){ return m_defaultCamera; }

	void render(Renderer* renderer);

	/** set camera scale */
	void setCameraScale(float sl);

	inline void setCameraPitch(float pitch){ m_pitch = pitch; }
	inline void setCameraYaw(float yaw){ m_yaw = yaw; updateCamera(0); }
	void setCameraTrans(const Vec3& v);
	void cameraReset();

	/**更新相机*/
	void updateCamera(float deta);

protected:
	Scene();
	virtual ~Scene();

	bool init();

protected:

	// ##场景默认相机， 主要渲染相机
	Camera* m_defaultCamera;
	std::vector<Camera*> m_cameras;

	std::vector<Light*> m_lights;

	float m_pitch;
	float m_yaw;

	friend class Camera;
	friend class Light;
};

NAMESPACE_END

#endif