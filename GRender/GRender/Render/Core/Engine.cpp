#include "GL/glew.h"

#include "Engine.h"
#include "../Comm/Utils.h"
#include "../Comm/MathFunction.h"

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"

#include <string>
#include <algorithm>
#include <iostream>

#if WIN32
#include <WinSock2.h>
#endif

USING_NAMESPACE_G

Engine* Engine::s_sharedEngine = nullptr;


Engine* Engine::getInstance()
{
	if (nullptr == s_sharedEngine){
		s_sharedEngine = new (std::nothrow) Engine();
		s_sharedEngine->init();
	}

	return s_sharedEngine;
}

Engine::Engine()
{

}

Engine::~Engine()
{
	delete m_renderer;
}

bool Engine::init()
{
	GLenum error = glewInit();
	if (error != GLEW_OK){
		G::log(">>>>> glewInit() failure , %d , %s \n", error, (char*)glGetString(error));
		return false;
	}

	m_runningScene = nullptr;
	m_sceneStack.reserve(15);
	m_winSize = Vec2(1280, 960);
	m_projection = Engine::Projection::_3D;
	m_lastUpdate = new timeval;

	this->initMatrixStack();
	this->setDefaultValues();
	this->setGLDefaultValues();
	
	m_renderer = new(std::nothrow) Renderer();

	return true;
}

void Engine::resize(int width, int height)
{
	m_winSize[0] = width;
	m_winSize[1] = height;

	this->setViewport();
}

void Engine::setProjection(Projection projection)
{
	m_projection = projection;

	auto size = m_winSize;
	this->setViewport();

	switch (projection)
	{
	case G::Engine::Projection::_2D:
	{

		// ##加载2d 投影矩阵
		loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		
		Mat4 orthoMatrix = G::createOrthographicMatrix(size(0), size(1), -1024, 1024);

		multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);

		// ##加载模型视图矩阵
		loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

		break;
	}
	case G::Engine::Projection::_3D:
	{
		loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
		
		Mat4 perspectiveMatrix = G::createPerspectiveMatrix(60, 1.0f * size(0) / size(1), 0.1f, 100000.0f);

		multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, perspectiveMatrix);

		Vec3 eye(0, 0, 100);
		Vec3 center(0, 0, 0);
		Vec3 up(0, 1, 0);

		Mat4 viewMatrix = G::createLookAtMatrix(eye, center, up);

		multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, viewMatrix);

		loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

		break;
	}
	default:
		break;
	}

}

void Engine::setViewport()
{
	glViewport(0, 0, m_winSize[0], m_winSize[1]);
}


void Engine::runWithScene(Scene *scene)
{
	if (nullptr == scene)
	{
		G::log(">>>>> Engine::runWithScene : scene == nullptr !");
		return;
	}

	pushScene(scene);
}

void Engine::pushScene(Scene *scene)
{
	if (nullptr == scene){
		G::log(">>>>> Engine::pushScene : scene == nullptr !");
		return;
	}

	m_sceneStack.push_back(scene);
	m_runningScene = scene;

	// ##enter
	if (m_runningScene){
		m_runningScene->onEnter();
	}
}

void Engine::popScene()
{
	if (nullptr != m_runningScene){
		// ###clearup
		m_runningScene->onExit();
		m_runningScene = nullptr;
	}

	m_sceneStack.pop_back();
	int c = m_sceneStack.size();

	if (c == 0){
		end();
	}
	else{
		m_runningScene = m_sceneStack.at(c - 1);
	}
}

void Engine::end()
{
	// cleanup data in next frame
	if (m_runningScene){
		m_runningScene->onExit();
	}
}

void Engine::drawScene()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	if (nullptr != m_runningScene){

		// clear draw stats
		// m_renderer->clearDrawStats();

		m_runningScene->render(m_renderer);
	}

	glDisable(GL_DEPTH_TEST);
	//m_renderer->render();

	//popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void Engine::mainLoop()
{
	// 是否需要在下一帧之前清楚数据
	// if(m_purgeEngineInNextLoop)
	// {
	// }
	// else{}

	// 1. update(float delta)
	calculateDeltaTime();

	// skip one flame when _deltaTime equal to zero.
	if (m_deltaTime < EPSILON){
		return;
	}

	//updateCamera(m_deltaTime);

	drawScene();
}

void Engine::calculateDeltaTime()
{
#if  WIN32

	timeval now;

	LARGE_INTEGER liTime, liFreq;
	QueryPerformanceFrequency(&liFreq);
	QueryPerformanceCounter(&liTime);

	now.tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
	now.tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - now.tv_sec * 1000000.0);

	//// new delta time. Re-fixed issue #1277
	//if (_nextDeltaTimeZero)
	//{
	//	_deltaTime = 0;
	//	_nextDeltaTimeZero = false;
	//}
	//else
	{
		m_deltaTime = (now.tv_sec - m_lastUpdate->tv_sec) + (now.tv_usec - m_lastUpdate->tv_usec) / 1000000.0f;
		m_deltaTime = max(0.0f, m_deltaTime);
	}

	*m_lastUpdate = now;

#endif
}


void Engine::setDefaultValues()
{

}

void Engine::setGLDefaultValues()
{
	this->setAlphaBlending(true);
	this->setDepthTest(true);

	//this->setProjection(m_projection);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void Engine::setAlphaBlending(bool on)
{
	
}

void Engine::setDepthTest(bool on)
{

}

Camera* Engine::getDefaultCamera()
{ 
	return m_runningScene->getDefaultCamera(); 
}


//////////////////////////////////////////////////////////////////////////
/// matrix stack
void Engine::initMatrixStack()
{
	while (!m_modelViewMatrixStack.empty())
	{
		m_modelViewMatrixStack.pop();
	}

	while (!m_projectionMatrixStack.empty())
	{
		m_projectionMatrixStack.pop();
	}

	while (!m_textureMatrixStack.empty())
	{
		m_textureMatrixStack.pop();
	}

	m_modelViewMatrixStack.push(Mat4::Identity());
	m_projectionMatrixStack.push(Mat4::Identity());
	m_textureMatrixStack.push(Mat4::Identity());
}


void Engine::pushMatrix(MATRIX_STACK_TYPE type)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		m_modelViewMatrixStack.push(m_modelViewMatrixStack.top());
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		m_projectionMatrixStack.push(m_projectionMatrixStack.top());
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
	{
		m_textureMatrixStack.push(m_textureMatrixStack.top());
	}
	else
	{
		G::log("unknow matrix stack type");
	}

}

void Engine::popMatrix(MATRIX_STACK_TYPE type)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_modelViewMatrixStack.pop();
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_projectionMatrixStack.pop();
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_textureMatrixStack.pop();
	}
	else
	{
		G::log( "unknow matrix stack type");
	}
}

void Engine::loadIdentityMatrix(MATRIX_STACK_TYPE type)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_modelViewMatrixStack.top() = Mat4::Identity();
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_projectionMatrixStack.top() = Mat4::Identity();
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_textureMatrixStack.top() = Mat4::Identity();
	}
	else
	{
		G::log( "unknow matrix stack type");
	}
}

void Engine::loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_modelViewMatrixStack.top() = mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_projectionMatrixStack.top() = mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_textureMatrixStack.top() = mat;
	}
	else
	{
		G::log("unknow matrix stack type");
	}
}

void Engine::multiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_modelViewMatrixStack.top() *= mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_projectionMatrixStack.top() *= mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_textureMatrixStack.top() *= mat;
	}
	else
	{
		G::log("unknow matrix stack type");
	}
}

const Mat4& Engine::getMatrix(MATRIX_STACK_TYPE type)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		return m_modelViewMatrixStack.top();
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		return m_projectionMatrixStack.top();
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
	{
		return m_textureMatrixStack.top();
	}

	G::log("unknow matrix stack type, will return modelview matrix instead");
	return  m_modelViewMatrixStack.top();
}

void Engine::resetMatrixStack()
{
	initMatrixStack();
}