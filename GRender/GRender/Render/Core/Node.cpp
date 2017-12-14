/************************************************************************/
/*  Node是Scene Graph场景树上所有元素的基类，在SceneGraph上的节点必须继承之
 *
 */
/************************************************************************/

#include "Node.h"
#include "../Comm/Utils.h"
#include "../Comm/MathFunction.h"

#include "Scene.h"
#include "Engine.h"
#include "GLProgramState.h"
#include "GLProgram.h"

#include "Ray.h"
#include <iostream>

USING_NAMESPACE_G


Node::Node()
: m_cascadeColorEnabled(false)
, m_parent(nullptr)
, m_glProgramState(nullptr)
, m_running(false)
, m_visible(true)
, m_transformUpdated(false)
, m_transformDirty(false)
, m_inverseDirty(false)
, m_aabb(nullptr)
, m_userData(nullptr)
, m_nodeFlagMask(_DEFAUT)
, m_priority(0)
{
	m_transform.setIdentity();
	m_modelViewTransform.setIdentity();
	m_inverse.setIdentity();

	m_rotation.setZero();
	m_position.setZero();
	m_scale.setOnes();
}

Node::~Node()
{

}

bool Node::init()
{
	return false;
}

std::string Node::getDescription() const
{
	return G::format(">>>> Node | Tag = %d ", m_tag);
}

/**
 * @@scale && position && rotation
 * @@{
 */
void Node::setScale(const Vec3& scale)
{
	if (m_scale == scale){
		return;
	}

	m_scale = scale;
	m_transformUpdated = true;
	m_transformDirty = true;
}

const Vec3& Node::getScale() const
{
	return m_scale;
}

void Node::setPosition(const Vec3& position)
{
	if (m_position == position){
		return;
	}

	m_position = position;
	m_transformUpdated = true;
	m_transformDirty = true;
}

const Vec3& Node::getPosition() const
{
	return m_position;
}

void Node::setPositionLocal(const Vec3& local)
{
	Mat3 mrot = getScene()->getCameraLookAtMatrix(m_position);

	Vec3 xaxis = Vec3(mrot.data()[0], mrot.data()[3], mrot.data()[6]);
	Vec3 yaxis = Vec3(mrot.data()[1], mrot.data()[4], mrot.data()[7]);
	Vec3 zaxis = Vec3(mrot.data()[2], mrot.data()[5], mrot.data()[8]);

	Vec3 offset(Vec3::Zero());
	if (local[0] != 0.f){
		offset += xaxis * local[0];
	}
	if (local[1] != 0.f){
		offset += yaxis * local[1];
	}
	if (local[2] != 0.f){
		offset += zaxis * local[2];
	}

	m_position += offset;
	m_transformUpdated = true;
	m_transformDirty = true;

	// ##更新aabb的位置
	if (m_aabb != nullptr){
		//m_aabb->transform(getNodeToWorldTransform());
		m_aabb->transformOffset(offset);
		updateBoundingBox();
	}
}


void Node::setRotation(const Vec3& rotation)
{
	if (m_rotation == rotation){
		return;
	}

	m_rotation = rotation;
	m_transformUpdated = true;
	m_transformDirty = true;
}

const Vec3& Node::getRotation() const
{
	return m_rotation;
}

void Node::setRotationLocal(const Vec3& rot)
{
	Scene* _scene = getScene();
	if (_scene == nullptr)
		return;

	Mat3 mrot = _scene->getCameraLookAtMatrix(m_position);
	Vec3 xaxis = Vec3(mrot.data()[0], mrot.data()[3], mrot.data()[6]);
	Vec3 yaxis = Vec3(mrot.data()[1], mrot.data()[4], mrot.data()[7]);
	Vec3 zaxis = Vec3(mrot.data()[2], mrot.data()[5], mrot.data()[8]);

	Vec3 offset(Vec3::Zero());
	if (rot[0] != 0.f){
		Angle_Axis x_axis(DEG_TO_RAD(rot[0]), xaxis);
		offset += x_axis.axis() * x_axis.angle();
	}
	if (rot[1] != 0.f){
		Angle_Axis y_axis(DEG_TO_RAD(rot[1]), yaxis);
		offset += y_axis.axis() * y_axis.angle();
	}
	if (rot[2] != 0.f){
		Angle_Axis z_axis(DEG_TO_RAD(rot[2]), zaxis);
		offset += z_axis.axis() * z_axis.angle();
	}

	m_rotation += offset;

	m_transformUpdated = true;
	m_transformDirty = true;

	//// ##更新aabb的位置
	//if (m_aabb != nullptr){
	//	m_aabb->transform(getNodeToWorldTransform());
	//	//m_aabb->rotationOffset(m_rotation);
	//	updateBoundingBox();
	//}
}

/**
 * @@}
 */

void Node::setVisible(bool visible)
{
	if (visible != m_visible){
		m_visible = visible;
		if (m_visible){
			m_transformUpdated = true;
			m_transformDirty = true;
		}
	}
}

bool Node::isVisible() const
{
	return m_visible;
}

void Node::addChild(Node *child, unsigned int priority)
{
	if (nullptr == child)
		return;

	this->addChild(child, child->getName(), priority);
}

void Node::addChild(Node *child, int tag, unsigned int priority)
{
	if (nullptr == child)
		return;

	this->addChildHelper(child, tag, "", true);
	m_priority = priority;
}

void Node::addChild(Node *child, const std::string &name, unsigned int priority)
{
	if (nullptr == child)
		return;

	this->addChildHelper(child, 0, name, false);
	m_priority = priority;
}

void Node::addChildHelper(Node* child, int tag, const std::string& name, bool setTag)
{
	if (m_children.empty()){
		m_children.reserve(4);
	}

	// ## 1.
	m_children.push_back(child);
	m_transformUpdated = true;
	m_transformDirty = true;

	// ## 2.
	if (setTag){
		child->setTag(tag);
	}
	else{
		child->setName(name);
	}

	// ## 3.
	child->setParent(this);

	// ## 4.如果当前节点正在运行，则加入的孩子节点立马初始化调用
	if (m_running){
		 child->onEnter();
		// child->onEnterTransitionDidFinish();
	}

	// ## 5.是否处于级联颜色更新
	if (m_cascadeColorEnabled){
		this->updateCascadeColor();
		this->updateCascadeOpacity();
	}
}


Node* Node::getChildByTag(int tag) const
{
	if (tag == Node::INVALID_TAG)
		return nullptr;

	for (auto &child : m_children){
		if (child && child->m_tag == tag){
			return child;
		}
	}

	return nullptr;
}

Node* Node::getChildByName(const std::string& name) const
{
	if (name.empty()){
		return nullptr;
	}

	// ##hash存储, 提升检索速度
	std::hash<std::string> h;
	unsigned int hash = h(name);

	for (const auto& child : m_children){

		if (child->m_hashName == hash && child->m_name.compare(name) == 0){
			return child;
		}
	}

	return nullptr;
}

int Node::getChildrenCount() const
{
	return m_children.size();
}

void Node::setParent(Node* parent)
{
	m_parent = parent;
	m_transformUpdated = true;
	m_transformDirty = true;
}

void Node::removeFromParent()
{
	this->removeFromParentAndCleanup(true);
}

void Node::removeFromParentAndCleanup(bool cleanup)
{
	if (nullptr != m_parent){
		m_parent->removeChild(this, cleanup);
	}
}

void Node::cleanup()
{
	for (const auto& child : m_children){
		child->cleanup();
	}

}

void Node::removeChild(Node* child, bool cleanup /*= true*/)
{
	if (m_children.empty()){
		return;
	}

	int index = -1;
	auto iter = std::find(m_children.begin(), m_children.end(), child);
	if (iter != m_children.end()){
		index = iter - m_children.begin();
	}

	if (-1 != index){

		// ## 是否正在运行
		if (m_running){
			// child->onExitTransitionDidStart()
			child->onExit();
		}

		if (cleanup){
			 child->cleanup();
		}

		child->setParent(nullptr);
		m_children.erase(iter);
	}
}

void Node::removeChildByTag(int tag, bool cleanup/* = true*/)
{
	if (Node::INVALID_TAG == tag){
		return;
	}

	Node *child = this->getChildByTag(tag);
	if (nullptr == child){
		G::log("Node::removeChildByTag(tag = %d): child not find !", tag);
	}
	else{
		this->removeChild(child, cleanup);
	}
}

void Node::removeChildByName(const std::string& name, bool cleanup /*= true*/)
{
	if (name.empty()){
		return;
	}

	Node *child = this->getChildByName(name);
	if (nullptr == child){
		G::log("Node::removeChildByName(name = %s): child not find !", name.c_str());
	}
	else{
		this->removeChild(child, cleanup);
	}
}

void Node::removeAllChildren()
{
	this->removeAllChildrenWithCleanup(true);
}

void Node::removeAllChildrenWithCleanup(bool cleanup /*= true*/)
{
	for (auto &child : m_children){
		if (m_running){
			//child->onExitTransitionDidStart();
			child->onExit();
		}

		if (cleanup){
			child->cleanup();
		}

		child->setParent(nullptr);
	}

	m_children.clear();
}


//////////////////////////////////////////////////////////////////////////

void Node::setTag(int tag)
{
	m_tag = tag;
}

int Node::getTag() const
{
	return m_tag;
}

void Node::setName(const std::string& name)
{
	m_name = name;

	std::hash<std::string> h;
	m_hashName = h(name);
}

std::string Node::getName() const
{
	return m_name;
}

void Node::setGLProgram(GLProgram* glProgram)
{
	
	if (m_glProgramState == nullptr || (m_glProgramState && m_glProgramState->getGLProgram() != glProgram)){
		delete m_glProgramState;
		m_glProgramState = nullptr;

		m_glProgramState = GLProgramState::getOrCreateWithGLProgram(glProgram);
	}
}

GLProgram* Node::getGLProgram() const
{
	return m_glProgramState ? m_glProgramState->getGLProgram() : nullptr;
}

void Node::setGLProgramState(GLProgramState* glProgramState)
{
	if (m_glProgramState != glProgramState){
		if (m_glProgramState)
			delete m_glProgramState;
		m_glProgramState = nullptr;
		m_glProgramState = glProgramState;
	}
}

//////////////////////////////////////////////////////////////////////////

void Node::draw(Renderer* renderer, const Mat4& transform, unsigned int flags)
{
	// implemtation
	// todo ...
}

void Node::visit(Renderer* renderer, const Mat4& parentTransform, unsigned int parentFlags)
{
	if (!m_visible){
		return;
	}

	// ##do transform
	unsigned int flags = this->processParentFlags(parentTransform, parentFlags);

	//
	//Engine* director = Engine::getInstance();
	//director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	//director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, m_modelViewTransform);

	// ##draw self
	this->draw(renderer, m_modelViewTransform, flags);

	// ##iter children
	for (auto &child : m_children){
		child->visit(renderer, m_modelViewTransform, flags);
	}

	//director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void Node::visit(Node* parent, std::vector<Node*>& outs, unsigned int nodeFlags)
{
	if (!m_visible){
		return;
	}

	// ##需要查询的节点
	if (nodeFlags == m_nodeFlagMask){
		outs.push_back(this);
	}

	// ##iter children
	for (auto &child : m_children){
		child->visit(this, outs, nodeFlags);
	}
}


void Node::visit(Node* parent, Ray* ray, std::vector<Node*>& outs, unsigned int nodeFlag, bool needOne)
{
	if (!m_visible){
		return;
	}

	if (ray == nullptr){
		return;
	}

	// ##需要查询的节点
	if (nodeFlag == m_nodeFlagMask && this->m_aabb){
		if (ray->intersects(*m_aabb)){
			outs.push_back(this);
			
			if (needOne){
				return;
			}
		}
	}

	// ##iter children
	for (auto &child : m_children){
		child->visit(this, ray, outs, nodeFlag);
	}
}

unsigned int Node::processParentFlags(const Mat4& parentTransform, unsigned int parentFlags)
{
	if (m_transformUpdated){
		m_transformUpdated = false;

		Mat4 ret = this->getNodeToParentTransform();
		m_modelViewTransform = parentTransform * ret;
	}

	return parentFlags;
}

void Node::update(float delta)
{
	// schedule update
	// todo ...
}

Scene* Node::getScene() const
{
	if (nullptr == m_parent){
		return nullptr;
	}

	auto sceneNode = m_parent;
	while (nullptr != sceneNode->m_parent){
		sceneNode = sceneNode->m_parent;
	}

	//return nullptr;
	return dynamic_cast<Scene*>(sceneNode);
}

AABB* Node::getBoundingBox() const
{
	return nullptr;
}

void Node::updateTransform()
{
	for (const auto& child : m_children){
		child->updateTransform();
	}
}

void Node::setNodeToParentTransform(const Mat4& transform)
{
	m_transform = transform;
	m_transformDirty = true;
	m_transformUpdated = true;
}

#include <QQuaternion>
#include <QMatrix4x4>
#include <QDebug>
const Mat4& Node::getNodeToParentTransform()
{
	if (m_transformDirty){

		m_transform.setIdentity();
		
		float angle = m_rotation.norm();
		Vec3 axis = m_rotation.normalized();

		Angle_Axis AA = Angle_Axis(angle, axis);
		Mat3 mrot = AA.toRotationMatrix();

		m_transform.data()[0] = mrot.data()[0] * m_scale[0];  
		m_transform.data()[1] = mrot.data()[1] * m_scale[0];
		m_transform.data()[2] = mrot.data()[2] * m_scale[0];
		m_transform.data()[3] = 0.0;

		m_transform.data()[4] = mrot.data()[3] * m_scale[1];
		m_transform.data()[5] = mrot.data()[4] * m_scale[1];
		m_transform.data()[6] = mrot.data()[5] * m_scale[1];
		m_transform.data()[7] = 0.0;

		m_transform.data()[8]  = mrot.data()[6] * m_scale[2];
		m_transform.data()[9]  = mrot.data()[7] * m_scale[2];
		m_transform.data()[10] = mrot.data()[8] * m_scale[2];
		m_transform.data()[11] = 0.0;

		m_transform.data()[12] = m_position[0]; 
		m_transform.data()[13] = m_position[1]; 
		m_transform.data()[14] = m_position[2]; 
		m_transform.data()[15] = 1.0;

		m_transformDirty = false;
	}

	return m_transform;
}

const Mat4& Node::getParentToNodeTransform()
{
	if (m_inverseDirty){
		m_inverse = m_transform.inverse();
		m_inverseDirty = false;
	}

	return m_inverse;
}

Mat4 Node::getNodeToWorldTransform()
{
	Mat4 t = this->getNodeToParentTransform();
	for (Node *p = m_parent; p != nullptr; p = p->getParent()){
		t = p->getNodeToParentTransform() * t;
	}

	return t;
}

Mat4 Node::getWorldToNodeTransform()
{
	return getNodeToWorldTransform().inverse();
}

Vec3 Node::convertToNodeSpace(const Vec3& worldPoint)
{
	Mat4 tmp = getWorldToNodeTransform();
	Vec4 vec4(worldPoint(0), worldPoint(1), worldPoint(2), 1.0);
	Vec4 ret = tmp * vec4;

	return Vec3(ret(0), ret(1), ret(2));
}

Vec3 Node::convertToWorldSpace(const Vec3& nodePoint)
{
	Mat4 tmp = getNodeToWorldTransform();
	Vec4 v(nodePoint(0), nodePoint(1), nodePoint(2), 1.0);
	Vec4 ret = tmp * v;

	return Vec3(ret(0), ret(1), ret(2));
}

void Node::onEnter()
{
	if (m_onEnterCallback){
		m_onEnterCallback();
	}

	for (const auto &child : m_children){
		child->onEnter();
	}

	m_running = true;
}

void Node::onExit()
{
	if (m_onExitCallback){
		m_onExitCallback();
	}

	m_running = false;

	for (const auto &child : m_children){
		child->onExit();
	}
}


void Node::setDisplayColor(const Color4& color)
{
	m_displayColor = color;
}

void Node::setDisplayColor(const Color3& color)
{
	m_displayColor[0] = color[0];
	m_displayColor[1] = color[1];
	m_displayColor[2] = color[2];
	m_displayColor[3] = 1.0f;
}

const Color4& Node::getDisplayColor() const
{
	return m_displayColor;
}
void Node::setCascadeColorEnabled(bool cascadeEnabled)
{
	m_cascadeColorEnabled = cascadeEnabled;
}
bool Node::getCascadeColorEnabled() const
{
	return m_cascadeColorEnabled;
}
void Node::updateDisplayColor(const Color4& parentColor)
{
	m_displayColor(0) = m_displayColor(0) * parentColor(0);
	m_displayColor(1) = m_displayColor(1) * parentColor(1);
	m_displayColor(2) = m_displayColor(2) * parentColor(2);
	m_displayColor(3) = m_displayColor(3) * parentColor(3);
	updateColor();

	if (m_cascadeColorEnabled){
		for (const auto &child : m_children){
			child->updateDisplayColor(m_displayColor);
		}
	}
}

void Node::updateCascadeColor()
{
	Color4 parentColor(Vec4::Ones());

	if (m_parent && m_parent->getCascadeColorEnabled()){
		parentColor = m_parent->getDisplayColor();
	}

	updateDisplayColor(parentColor);

}

void Node::updateCascadeOpacity()
{

}