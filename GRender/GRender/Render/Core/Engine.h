/************************************************************************/
/* 
* 整个引擎的管理类
*/
/************************************************************************/

#ifndef  __ENGINE_H__
#define  __ENGINE_H__

#include "../Comm/Macors.h"
#include "../Comm/Math.h"
#include <stack>


NAMESPACE_BEGIN


class Scene;
class Renderer;
class Camera;

/**
 * 矩阵类型
 */
enum class MATRIX_STACK_TYPE
{
	MATRIX_STACK_MODELVIEW,
	MATRIX_STACK_PROJECTION,
	MATRIX_STACK_TEXTURE
};

class Engine
{
public:

	// ##投影类型
	enum class Projection
	{
		_2D,
		_3D,
		DEFAULT = _3D
	};

public:

	/**
	 * singleton
	 */
	static Engine* getInstance();

	/**
	 * constructor
	 * destructor
	 * initialize
	 */
	Engine();
	virtual ~Engine();

	/**
	 * 初始化 需要做opengl相关的初始工作
	 */
	virtual bool init();

	/** 重置视口大小 */
	virtual void resize(int width, int height);

	/** Get current running Scene. Director can only run one Scene at a time */
	inline Scene* getRunningScene(){ return m_runningScene; }

	/**Get Default camera*/
	Camera* getDefaultCamera();

	/** Sets an OpenGL projection*/
	inline Projection getProjection() { return m_projection; }
	void setProjection(Projection projection);

	/** Sets the glViewport*/
	void setViewport();
	
	/** 
	*returns the size of the OpenGL view in points.
	*/
	Vec2 getWinSize() const{ return m_winSize; };

	/** Enters the Director's main loop with the given Scene.
	* Call it to run only your FIRST scene.
	* Don't call it if there is already a running scene.
	*
	*/
	void runWithScene(Scene *scene);

	/** Suspends the execution of the running scene, pushing it on the stack of suspended scenes.
	* The new scene will be executed.
	* Try to avoid big stacks of pushed scenes to reduce memory allocation.
	* ONLY call it if there is a running scene.
	*/
	void pushScene(Scene *scene);

	/** Pops out a scene from the stack.
	* This scene will replace the running one.
	* The running scene will be deleted. If there are no more scenes in the stack the execution is terminated.
	* ONLY call it if there is a running scene.
	*/
	void popScene();


	/** Ends the execution, releases the running scene.
	It doesn't remove the OpenGL view from its parent. You have to do it manually.
	*/
	void end();


	/** Draw the scene.
	This method is called every frame. Don't call it manually.
	*/
	void drawScene();

	/** sets the default values based on the Configuration info */
	void setDefaultValues();

	// OpenGL Helper

	/** sets the OpenGL default values */
	void setGLDefaultValues();

	/** enables/disables OpenGL alpha blending */
	void setAlphaBlending(bool on);

	/** enables/disables OpenGL depth test */
	void setDepthTest(bool on);

	virtual void mainLoop();

	/** 
	*Returns the Renderer
	*/
	Renderer* getRenderer() const { return m_renderer; }

	// ##matrix 
	void pushMatrix(MATRIX_STACK_TYPE type);
	void popMatrix(MATRIX_STACK_TYPE type);
	void loadIdentityMatrix(MATRIX_STACK_TYPE type);
	void loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat);
	void multiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat);
	const Mat4& getMatrix(MATRIX_STACK_TYPE type);
	void resetMatrixStack();

protected:
	void initMatrixStack();

	void calculateDeltaTime();

protected:

	// ##矩阵栈， 用于绘制渲染矩阵操作
	std::stack<Mat4> m_modelViewMatrixStack;
	std::stack<Mat4> m_projectionMatrixStack;
	std::stack<Mat4> m_textureMatrixStack;

	/* The running scene */
	Scene *m_runningScene;

	/** The scene vector */
	std::vector<Scene*> m_sceneStack;

	/* projection used: 3D or 2D */
	Projection m_projection;

	/* window size*/
	Vec2 m_winSize;

	/* Renderer for the Director */
	Renderer* m_renderer;

	/** 时间间隔*/
	struct timeval {
		long    tv_sec;         /* seconds */
		long    tv_usec;        /* and microseconds */

		/* constructor */
		timeval() :tv_sec(0), tv_usec(0){}

		/* assignment */
		void operator =  (const timeval& val){
			tv_sec = val.tv_sec;
			tv_usec = val.tv_usec;
		}
	};

	float m_deltaTime;
	timeval *m_lastUpdate;

private:
	static Engine* s_sharedEngine;
};

NAMESPACE_END


#endif