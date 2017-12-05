/************************************************************************/
/*  Node是Scene Graph场景树上所有元素的基类，在SceneGraph上的节点必须继承之
 *
 */
/************************************************************************/

#ifndef  __NODE_H__
#define  __NODE_H__


#include "../Comm/Math.h"
#include "Ref.h"

#include <algorithm>
#include <functional>

NAMESPACE_BEGIN

class GLProgram;
class GLProgramState;
class Renderer;
class Scene;
class AABB;

class Node : public Ref
{
public:

	static const int INVALID_TAG = -1;

	/**
	* Gets the description string. It makes debugging easier.
	* @return A string
	*/
	virtual std::string getDescription() const;

	/**
	* Sets the scale (x,y,z) of the node.
	* It is a scaling factor that multiplies the x, y and z of the node and its children.
	* @param scale     The scale factor for X , Y, Z axis.
	*/
	virtual void setScale(const Vec3& scale);
	virtual const Vec3& getScale() const;


	/**
	* Sets the position (x,y,z) of the node in its parent's coordinate system.
	* @param position  The position (x,y,z) of the node in OpenGL coordinates
	*/
	virtual void setPosition(const Vec3& position);
	virtual const Vec3& getPosition() const;


	/**
	* Sets the rotation (X,Y,Z) in degrees.
	* Useful for 3d rotations
	*/
	virtual void setRotation(const Vec3& rotation);
	virtual const Vec3& getRotation() const;

	/**
	* Sets whether the node is visible
	* The default value is true, a node is default to visible
	* @param visible   true if the node is visible, false if the node is hidden.
	*/
	virtual void setVisible(bool visible);
	virtual bool isVisible() const;


	/**
	* Adds a child to the container .
	* @param child A child node
	* @param tag tag of A child
	* @param name name of A child
	*/
	virtual void addChild(Node *child);
	virtual void addChild(Node *child, int tag);
	virtual void addChild(Node *child, const std::string &name);

	/**
	* Gets a child from the container with its tag
	* @param tag(name)   An identifier to find the child node.
	* @return a Node object whose tag equals to the input parameter
	*/
	virtual Node* getChildByTag(int tag) const;
	virtual Node* getChildByName(const std::string& name) const;


	/**
	* Returns the array of the node's children
	*
	* @return the array the node's children
	*/
	virtual std::vector<Node*>& getChildren() { return m_children; }
	virtual const std::vector<Node*>& getChildren() const { return m_children; }

	/**
	* Returns the amount of children
	*
	* @return The amount of children.
	*/
	virtual int getChildrenCount() const;

	/**
	* Sets(Gets) the parent node
	*
	* @param parent    A pointer to the parent node
	*/
	virtual void setParent(Node* parent);
	virtual Node* getParent() { return m_parent; }
	virtual const Node* getParent() const{ return m_parent; }

	/**
	* Removes this node itself from its parent node with a cleanup.
	* If the node orphan, then nothing happens.
	* @see `removeFromParentAndCleanup(bool)`
	*/
	virtual void removeFromParent();

	/**
	* Removes this node itself from its parent node.
	* If the node orphan, then nothing happens.
	* @param cleanup   true if all actions and callbacks on this node should be removed, false otherwise.
	*/
	virtual void removeFromParentAndCleanup(bool cleanup);

	/**
	* Removes a child from the container. It will also cleanup all running actions depending on the cleanup parameter.
	*
	* @param child     The child node which will be removed.
	* @param cleanup   true if all running actions and callbacks on the child node will be cleanup, false otherwise.
	*/
	virtual void removeChild(Node* child, bool cleanup = true);
	virtual void removeChildByTag(int tag, bool cleanup = true);
	virtual void removeChildByName(const std::string& name, bool cleanup = true);
	virtual void removeAllChildren();
	virtual void removeAllChildrenWithCleanup(bool cleanup = true);

	/**
	* Stops all running actions and schedulers
	*/
	virtual void cleanup();

	/**
	* Changes the tag(name) that is used to identify the node easily.
	*
	* Please refer to getTag(getName) for the sample code.
	*
	* @param tag(name)   A integer that identifies the node.
	*/
	virtual void setTag(int tag);
	virtual int getTag() const;

	virtual void setName(const std::string& name);
	virtual std::string getName() const;

	/**
	* Sets a custom user data pointer
	*
	* You can set everything in UserData pointer, a data block, a structure or an object, etc.
	*
	* @param userData  A custom user data pointer
	*/
	virtual void setUserData(void* userData){ m_userData = userData; }
	virtual void* getUserData() { return m_userData; }
	virtual const void* getUserData() const{ return m_userData; }


	/**
	* setter/getter the GLProgram/GLProgramState (shader) currently used for this node
	*/
	virtual void setGLProgram(GLProgram* glProgram);
	GLProgram* getGLProgram() const;
	virtual void setGLProgramState(GLProgramState* glProgramState);
	GLProgramState* getGLProgramState() const { return m_glProgramState; }

	/**
	* Returns whether or not the node is "running".
	*
	* If the node is running it will accept event callbacks like onEnter(), onExit(), update()
	*
	* @return Whether or not the node is running.
	*/
	virtual bool isRunning() const { return m_running; }

	/**
	* Override this method to draw your own node.
	* The following GL states will be enabled by default:
	* - `glEnableClientState(GL_VERTEX_ARRAY);`
	* - `glEnableClientState(GL_COLOR_ARRAY);`
	* - `glEnableClientState(GL_TEXTURE_COORD_ARRAY);`
	* - `glEnable(GL_TEXTURE_2D);`
	* AND YOU SHOULD NOT DISABLE THEM AFTER DRAWING YOUR NODE
	* But if you enable any other GL state, you should disable it after drawing your node.
	*/
	virtual void draw(Renderer* renderer, const Mat4& transform, unsigned int flags);

	/**
	* Visits this node's children and draw them recursively.
	*/
	virtual void visit(Renderer* renderer, const Mat4& parentTransform, unsigned int parentFlags);

	/*
	* Update method will be called automatically every frame if "scheduleUpdate" is called, and the node is "live"
	*/
	virtual void update(float delta);


	/** Returns the Scene that contains the Node.
	 * It returns `nullptr` if the node doesn't belong to any Scene.
	 * This function recursively calls parent->getScene() until parent is a Scene object. The results are not cached. 
	 * It is that the user caches the results in case this functions is being used inside a loop.
	 */
	virtual Scene* getScene() const;

	/**
	* Returns an AABB (axis-aligned bounding-box) in its parent's coordinate system.
	*/
	virtual AABB* getBoundingBox() const;

	/**
	 * @{
	 * @name Transformations
	 */

	/**
	* Calls children's updateTransform() method recursively.
	*/
	virtual void updateTransform();

	/**
	* Returns/Set the matrix that transform the node's (local) space coordinates into the parent's space coordinates.
	*/
	virtual void setNodeToParentTransform(const Mat4& transform);
	virtual const Mat4& getNodeToParentTransform();

	/**
	* Returns the matrix that transform parent's space coordinates to the node's (local) space coordinates.
	*/
	virtual const Mat4& getParentToNodeTransform();

	/**
	* Returns the matrix that transform the node's(local) space coordinates into the world's space coordinates.
	*/
	virtual Mat4 getNodeToWorldTransform();

	/**
	* Returns the matrix that transform the world's(world) space coordinates into the node's space coordinates.
	*/
	virtual Mat4 getWorldToNodeTransform();


	Vec3 convertToNodeSpace(const Vec3& worldPoint);
	Vec3 convertToWorldSpace(const Vec3& nodePoint);


	/**
	* Event callback that is invoked every time when Node enters the 'stage'.
	* If the Node enters the 'stage' with a transition, this event is called when the transition starts.
	* During onEnter you can't access a "sister/brother" node.
	* If you override onEnter, you shall call its parent's one, e.g., Node::onEnter().
	*/
	virtual void onEnter();

	/**
	* Event callback that is invoked every time the Node leaves the 'stage'.
	* If the Node leaves the 'stage' with a transition, this event is called when the transition finishes.
	* During onExit you can't access a sibling node.
	* If you override onExit, you shall call its parent's one, e.g., Node::onExit().
	*/
	virtual void onExit();

	/**
	 * @}
	 */

	/**
	 * display color
	 */
	virtual void setDisplayColor(const Color3& color);
	virtual const Color3& getDisplayColor() const;
	virtual void setCascadeColorEnabled(bool cascadeEnabled);
	virtual bool getCascadeColorEnabled() const;
	virtual void updateDisplayColor(const Color3& parentColor);


	virtual void updateColor(){};


	void setOnEnterCallback(const std::function<void()>& callback){ m_onEnterCallback = callback; }
	void setOnExitCallback(const std::function<void()>& callback){ m_onExitCallback = callback; }

	const std::function<void()>& getOnEnterCallback() const{ return m_onEnterCallback; }
	const std::function<void()>& getOnExitCallback() const{ return m_onExitCallback; }


protected:
	Node();
	virtual ~Node();

	virtual bool init();

	unsigned int processParentFlags(const Mat4& parentTransform, unsigned int parentFlags);

	void addChildHelper(Node* child, int tag, const std::string& name, bool setTag);

	void updateCascadeColor();
	void updateCascadeOpacity();

protected:
	/**
	 * @Attribute
	 * @{
	 */

	// ##是否在运行
	bool m_running;

	// ##是否隐藏
	bool m_visible;

	// ##是否Transform需要更新
	bool m_transformUpdated;
	bool m_transformDirty;
	bool m_inverseDirty;

	// ##节点tag
	int m_tag;

	// ##节点name
	std::string m_name;
	unsigned int m_hashName; 

	// ##节点的位置
	Vec3 m_position;			

	// ##节点的旋转(angle - axis)
	Vec3 m_rotation;			

	// ##节点的缩放
	Vec3 m_scale;				

	// ##节点的模型视图矩阵
	Mat4 m_modelViewTransform;	
	Mat4 m_transform;
	Mat4 m_inverse;

	// ##box
	AABB * m_aabb;

	// ##孩子节点集合
	std::vector<Node*> m_children;

	// ##父亲节点
	Node* m_parent;

	// ##用户赋予的数据
	void* m_userData;

	// ##OpenGl 程序状态
	GLProgramState *m_glProgramState;

	// ##显示当前节点的颜色&&透明
	Color3 m_displayColor;
	float m_displayOpacity;
	bool m_cascadeColorEnabled;

	std::function<void()> m_onEnterCallback;
	std::function<void()> m_onExitCallback;

	// ##schedule
	// Scheduler *m_scheduler;

	// ##eventDispatcher
	// EventDispatcher* m_eventDispatcher;

	/**
	 * @}
	 */
};

NAMESPACE_END

#endif
