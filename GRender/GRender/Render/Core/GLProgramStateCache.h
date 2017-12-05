/************************************************************************/
/* 
* Program state cache
*/
/************************************************************************/

#ifndef  __GLPROGRAMSTATECACHE_H__
#define  __GLPROGRAMSTATECACHE_H__


#include "../Comm/Macors.h"

#include <unordered_map>

NAMESPACE_BEGIN

class GLProgram;
class GLProgramState;

class GLProgramStateCache
{

public :
	static GLProgramStateCache* getInstance();

	/**
	* get GLProgramState
	*/
	GLProgramState* getGLProgramState(GLProgram* program);


	void removeUnusedGLProgramState();
	void removeAllGLProgramState();


protected:
	GLProgramStateCache();
	~GLProgramStateCache();

	std::unordered_map<GLProgram*, GLProgramState*> m_glProgramStates;

	static GLProgramStateCache* s_instance;

};

NAMESPACE_END

#endif