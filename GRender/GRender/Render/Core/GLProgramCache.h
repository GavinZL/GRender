/************************************************************************/
/* 
* GLProgram Cache 管理类
*/
/************************************************************************/

#ifndef  __GLPROGRAMCACHE_H__
#define  __GLPROGRAMCACHE_H__

#include "../Comm/Macors.h"

#include <unordered_map>


NAMESPACE_BEGIN

//static const char* SHADER_NAME_POSITION;
//static const char* SHADER_NAME_POSITION_COLOR;
//static const char* SHADER_NAME_POSITION_TEXTURE_COLOR;
//static const char* SHADER_NAME_POSITION_NORMAL_COLOR;
//static const char* SHADER_NAME_POSITION_NORMAL_TEXTURE_COLOR;

/**
* 默认shader 加载
*/
enum ShaderType
{
	Shader_Name_Position,
	Shader_Name_Position_Color,
	Shader_Name_Position_Texture_Color,
	Shader_Name_Position_Normal_Color,
	Shader_Name_Position_Normal_Texture,
	Shader_Name_Position_Normal_Texture_Color
};

class GLProgram;

class GLProgramCache 
{
public:
	/**
	* constructor && destructor
	*/
	GLProgramCache();
	virtual ~GLProgramCache();

	/**
	* singleton
	*/
	static GLProgramCache* getInstance();

	/**
	* loads(reload) the default shaders
	*/
	void loadDefaultGLPrograms();
	void reloadDefaultGLPrograms();

	/**
	* return a GL Program for a give key
	*/
	GLProgram* getGLProgram(const std::string& key);

	/**
	* add a GL Program to the cache
	*/
	void addGLProgram(GLProgram* program, const std::string& key);


private:

	/**
	* inner function for default program
	*/
	bool init();
	void loadDefaultGLProgram(GLProgram* program, int type);


private:
	/**
	* caches programs
	*/
	std::unordered_map<std::string, GLProgram*> m_programs;


	static GLProgramCache *m_instanceCache;

};

NAMESPACE_END


#endif