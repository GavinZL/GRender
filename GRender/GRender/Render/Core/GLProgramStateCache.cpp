#include "GLProgramStateCache.h"

#include "GLProgram.h"
#include "GLProgramState.h"


USING_NAMESPACE_G;

GLProgramStateCache* GLProgramStateCache::s_instance = nullptr;

GLProgramStateCache* GLProgramStateCache::getInstance()
{
	if (nullptr == s_instance){
		s_instance = new (std::nothrow) GLProgramStateCache;
	}

	return s_instance;
}

GLProgramStateCache::GLProgramStateCache()
{

}

GLProgramStateCache::~GLProgramStateCache()
{
	m_glProgramStates.clear();
}


GLProgramState* GLProgramStateCache::getGLProgramState(GLProgram* program)
{
	const auto & iter = m_glProgramStates.find(program);
	if (iter != m_glProgramStates.end()){
		return iter->second;
	}

	auto ret = new (std::nothrow) GLProgramState;
	if (ret && ret->init(program)){
		m_glProgramStates[program] = ret;
		return ret;
	}

	return ret;
}

void GLProgramStateCache::removeUnusedGLProgramState()
{
	for (auto it = m_glProgramStates.cbegin(); it != m_glProgramStates.cend();){
		auto value = it->second;

		// ##delete unused 
		//if (value->getReferenceCount() == 1) {
		//	G::log("cocos2d: GLProgramStateCache: removing unused GLProgramState");

		//	value->release();
		//	m_glProgramStates.erase(it++);
		//}
		//else {
		//	++it;
		//}
	}
}

void GLProgramStateCache::removeAllGLProgramState()
{
	m_glProgramStates.clear();
}