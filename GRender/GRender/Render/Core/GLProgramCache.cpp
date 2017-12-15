#include "GL/glew.h"
#include "GLProgramCache.h"
#include "GLProgram.h"

#include "../Comm/Utils.h"

USING_NAMESPACE_G

const char* shader_Position_vert = "shader/position_vert.shader";
const char* shader_Position_frag = "shader/position_frag.shader";

const char* shader_Position_Color_vert = "shader/position_color_vert.shader";
const char* shader_Position_Color_frag = "shader/position_color_frag.shader";
// temp
const char* shader_Position_Texture_Color_vert = "shader/position_color_vert.shader";
const char* shader_Position_Texture_Color_frag = "shader/position_color_frag.shader";

const char* shader_Position_Normal_Color_vert = "shader/position_normal_color_vert2.shader";
const char* shader_Position_Normal_Color_frag = "shader/position_normal_color_frag2.shader";

const char* shader_Position_Normal_Texture_vert = "shader/position_normal_texture_vert2.shader";
const char* shader_Position_Normal_Texture_frag = "shader/position_normal_texture_frag2.shader";

const char* shader_Position_Normal_Texture_Color_vert = "shader/position_normal_color_texture_vert.shader";
const char* shader_Position_Normal_Texture_Color_frag = "shader/position_normal_color_texture_frag.shader";


GLProgramCache *GLProgramCache::m_instanceCache = nullptr;

GLProgramCache* GLProgramCache::getInstance()
{
	if (m_instanceCache == nullptr){
		m_instanceCache = new (std::nothrow) GLProgramCache();
		if (!m_instanceCache->init()){
			return nullptr;
		}
	}
	return m_instanceCache;
}

GLProgramCache::GLProgramCache()
	: m_programs()
{
}

GLProgramCache::~GLProgramCache()
{
	for (auto it = m_programs.begin(); it != m_programs.end(); ++it){
		//(it->second)->release();
	}

	m_programs.clear();
}

void GLProgramCache::loadDefaultGLPrograms()
{
	// ##position shader
	GLProgram *p = new (std::nothrow) GLProgram;
	this->loadDefaultGLProgram(p, Shader_Name_Position);
	m_programs.insert(std::make_pair(GLProgram::SHADER_NAME_POSITION, p));

	p = new (std::nothrow) GLProgram;
	this->loadDefaultGLProgram(p, Shader_Name_Position_Color);
	m_programs.insert(std::make_pair(GLProgram::SHADER_NAME_POSITION_COLOR, p));

	p = new (std::nothrow) GLProgram;
	this->loadDefaultGLProgram(p, Shader_Name_Position_Normal_Color);
	m_programs.insert(std::make_pair(GLProgram::SHADER_NAME_POSITION_NORMAL_COLOR, p));

	p = new (std::nothrow) GLProgram;
	this->loadDefaultGLProgram(p, Shader_Name_Position_Texture_Color);
	m_programs.insert(std::make_pair(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR, p));

	p = new (std::nothrow) GLProgram;
	this->loadDefaultGLProgram(p, Shader_Name_Position_Normal_Texture);
	m_programs.insert(std::make_pair(GLProgram::SHADER_NAME_POSITION_NORMAL_TEXTURE, p));

	p = new (std::nothrow) GLProgram;
	this->loadDefaultGLProgram(p, Shader_Name_Position_Normal_Texture_Color);
	m_programs.insert(std::make_pair(GLProgram::SHADER_NAME_POSITION_NORMAL_TEXTURE_COLOR, p));

}

void GLProgramCache::reloadDefaultGLPrograms()
{
	GLProgram *p = getGLProgram(GLProgram::SHADER_NAME_POSITION);
	p->reset();
	loadDefaultGLProgram(p, Shader_Name_Position);

	p = getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR);
	p->reset();
	loadDefaultGLProgram(p, Shader_Name_Position_Color);

	p = getGLProgram(GLProgram::SHADER_NAME_POSITION_NORMAL_COLOR);
	p->reset();
	loadDefaultGLProgram(p, Shader_Name_Position_Normal_Color);

	p = getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR);
	p->reset();
	loadDefaultGLProgram(p, Shader_Name_Position_Texture_Color);

	p = getGLProgram(GLProgram::SHADER_NAME_POSITION_NORMAL_TEXTURE_COLOR);
	p->reset();
	loadDefaultGLProgram(p, Shader_Name_Position_Normal_Texture_Color);

}

GLProgram* GLProgramCache::getGLProgram(const std::string& key)
{
	auto it = m_programs.find(key);
	if (it != m_programs.end()){
		return it->second;
	}

	return nullptr;
}

void GLProgramCache::addGLProgram(GLProgram* program, const std::string& key)
{
	auto prev = getGLProgram(key);
	if (prev == program){
		return;
	}

	m_programs.erase(key);
	m_programs[key] = program;
}

bool GLProgramCache::init()
{
	loadDefaultGLPrograms();
	return true;
}

void GLProgramCache::loadDefaultGLProgram(GLProgram* program, int type)
{
	bool bret = false;
	switch (type)
	{
	case Shader_Name_Position:
	{
		bret = program->initProgramWithFile(shader_Position_vert, shader_Position_frag);
	}
	break;
	case Shader_Name_Position_Color:
	{
		bret = program->initProgramWithFile(shader_Position_Color_vert, shader_Position_Color_frag);
	}
	break;
	case Shader_Name_Position_Normal_Color:
	{
		bret = program->initProgramWithFile(shader_Position_Normal_Color_vert, shader_Position_Normal_Color_frag);
	}
	break;
	case Shader_Name_Position_Normal_Texture_Color:
	{
		bret = program->initProgramWithFile(shader_Position_Normal_Texture_Color_vert, shader_Position_Normal_Texture_Color_frag);
	}
	break;
	case Shader_Name_Position_Texture_Color:
	{
		bret = program->initProgramWithFile(shader_Position_Texture_Color_vert, shader_Position_Texture_Color_frag);
	}
	break;
	case Shader_Name_Position_Normal_Texture:
	{
		bret = program->initProgramWithFile(shader_Position_Normal_Texture_vert, shader_Position_Normal_Texture_frag);
	}
	default:
		break;
	}

	if (bret){
		program->link();
		program->updateUniforms();
	}
}