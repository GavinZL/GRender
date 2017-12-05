#include "GL/glew.h"

#include "GLProgramState.h"

#include "GLProgram.h"
#include "GLProgramCache.h"
#include "GLProgramStateCache.h"

#include "Texture2D.h"
#include "../GLStateCache.h"

#include "../Comm/Utils.h"


USING_NAMESPACE_G

UniformValue::UniformValue()
: m_uniform(nullptr)
, m_glProgram(nullptr)
, m_useCallback(false)
{

}

UniformValue::UniformValue(Uniform* uniform, GLProgram* glProgram)
	: m_uniform(uniform)
	, m_glProgram(glProgram)
	, m_useCallback(false)
{

}

UniformValue::~UniformValue()
{
	if (m_useCallback){
		delete m_value.callback;
	}
}

void UniformValue::setInt(int val)
{
	if (m_uniform->type == GL_INT){
		m_value.intVaule = val;
		m_useCallback = false;
	}
}

void UniformValue::setFloat(int val)
{
	if (m_uniform->type == GL_FLOAT){
		m_value.floatVaule = val;
		m_useCallback = false;
	}
}

void UniformValue::setVec2(const Vec2& val)
{
	if (GL_FLOAT_VEC2 == m_uniform->type){
		m_value.v2Value[0] = val[0];
		m_value.v2Value[1] = val[1];
		m_useCallback = false;
	}
}

void UniformValue::setVec3(const Vec3& val)
{
	if (GL_FLOAT_VEC3 == m_uniform->type){
		m_value.v3Value[0] = val[0];
		m_value.v3Value[1] = val[1];
		m_value.v3Value[2] = val[2];
		m_useCallback = false;
	}
}

void UniformValue::setVec4(const Vec4& val)
{
	if (GL_FLOAT_VEC4 == m_uniform->type){
		m_value.v4Value[0] = val[0];
		m_value.v4Value[1] = val[1];
		m_value.v4Value[2] = val[2];
		m_value.v4Value[3] = val[3];
		m_useCallback = false;
	}
}

void UniformValue::setMat4(const Mat4& mat)
{
	if (GL_FLOAT_MAT4 == m_uniform->type){
		memcpy(m_value.mat4Value, &mat, sizeof(m_value.mat4Value));
		m_useCallback = false;
	}
}

void UniformValue::setTexture(GLuint textureId, GLuint textureUint)
{
	if (GL_SAMPLER_2D == m_uniform->type){
		m_value.m_tex.textureId = textureId;
		m_value.m_tex.textureUnit = textureUint;
		m_useCallback = false;
	}
}

void UniformValue::setCallback(const std::function<void(GLProgram*, Uniform*)> &callback)
{
	if (m_useCallback){
		delete m_value.callback;
	}

	m_value.callback = new std::function<void(GLProgram*, Uniform*)>();
	*m_value.callback = callback;

	m_useCallback = true;
}

// apply the uniform's value
void UniformValue::apply()
{
	if (m_useCallback){
		(*m_value.callback)(m_glProgram, m_uniform);
	}
	else
	{
		switch (m_uniform->type)
		{
		case GL_INT:
			m_glProgram->setUniformLocationWith1i(m_uniform->location, m_value.intVaule);
			break;
		case GL_FLOAT:
			m_glProgram->setUniformLocationWith1f(m_uniform->location, m_value.floatVaule);
			break;
		case GL_FLOAT_VEC2:
			m_glProgram->setUniformLocationWith2f(m_uniform->location, m_value.v2Value[0], m_value.v2Value[1]);
			break;
		case GL_FLOAT_VEC3:
			m_glProgram->setUniformLocationWith3f(m_uniform->location, m_value.v3Value[0], m_value.v3Value[1], m_value.v3Value[2]);
			break;
		case GL_FLOAT_VEC4:
			m_glProgram->setUniformLocationWith4f(m_uniform->location, m_value.v4Value[0], m_value.v4Value[1], m_value.v4Value[2], m_value.v4Value[3]);
			break;
		case GL_FLOAT_MAT4:
			m_glProgram->setUniformLocationWithMatrix4fv(m_uniform->location, (GLfloat*)m_value.mat4Value,1);
			break;
		case GL_SAMPLER_2D:
			m_glProgram->setUniformLocationWith1i(m_uniform->location, m_value.m_tex.textureUnit);
			G::bindTexture2DN(m_value.m_tex.textureUnit, m_value.m_tex.textureId);
			break;
		default:
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

AttributeValue:: AttributeValue()
	: m_attribute(nullptr)
	, m_useCallback(false)
	, m_enabled(false)
{

}

AttributeValue::AttributeValue(Attribute *attribute)
	: m_attribute(attribute)
	, m_useCallback(false)
	, m_enabled(false)
{

}

AttributeValue::~AttributeValue()
{
	if (m_useCallback){
		delete m_value.callback;
	}
}

void AttributeValue::setPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer)
{
	m_value.ptr.size = size;
	m_value.ptr.type = type;
	m_value.ptr.normalized = normalized;
	m_value.ptr.stride = stride;
	m_value.ptr.pointer = pointer;

	m_enabled = true;
}

void AttributeValue::setCallback(const std::function<void(Attribute*)> &callback)
{
	m_value.callback = new std::function<void(Attribute*)>();
	*m_value.callback = callback;
	m_useCallback = true;
	m_enabled = true;
}

void AttributeValue::apply()
{
	if (m_enabled){
		if (m_useCallback){
			(*m_value.callback)(m_attribute);
		}
		else
		{
			glVertexAttribPointer(m_attribute->location,
				m_value.ptr.size,
				m_value.ptr.type,
				m_value.ptr.normalized,
				m_value.ptr.stride,
				m_value.ptr.pointer);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

GLProgramState::GLProgramState()
	: m_uniformAttributeValueDirty(true)
	, m_textureUnitIndex(1)
	, m_attributeFlags(0)
	, m_glProgram(nullptr)
{
}

GLProgramState::~GLProgramState()
{
	if (nullptr != m_glProgram){
		delete m_glProgram;
	}
}

GLProgramState* GLProgramState::create(GLProgram* glProgram)
{
	GLProgramState* ret = new (std::nothrow) GLProgramState;
	if (ret && ret->init(glProgram)){
		return ret;
	}

	return nullptr;
}

GLProgramState* GLProgramState::getOrCreateWithGLProgram(GLProgram* glProgram)
{
	GLProgramState* ret = GLProgramStateCache::getInstance()->getGLProgramState(glProgram);
	return ret;
}

GLProgramState* GLProgramState::getOrCreateWithGLProgramName(const std::string &glProgramName)
{
	GLProgram *glProgram = GLProgramCache::getInstance()->getGLProgram(glProgramName);
	if (glProgram){
		return getOrCreateWithGLProgram(glProgram);
	}

	return nullptr;
}

bool GLProgramState::init(GLProgram* program)
{
	if (nullptr == program){
		G::log("invalid shader program");
		return false;
	}

	m_glProgram = program;

	for (auto &attrib : m_glProgram->m_attributes){
		AttributeValue value(&attrib.second);
		m_attributes[attrib.first] = value;
	}

	for (auto &uniform : m_glProgram->m_uniforms){
		UniformValue vaule(&uniform.second, m_glProgram);

		m_uniformsById[uniform.second.location] = vaule;
		m_uniformsByName[uniform.first] = uniform.second.location;
	}

	return true;
}

void GLProgramState::resetGLProgram()
{
	if (nullptr != m_glProgram){
		delete m_glProgram;
	}

	m_attributes.clear();
	m_uniformsById.clear();
	m_uniformsByName.clear();
	m_textureUnitIndex = 1;
}

void GLProgramState::apply(const Mat4& modelView)
{
	applyGLProgram(modelView);

	applyAttributes();

	applyUniforms();
}

void GLProgramState::applyGLProgram(const Mat4& movelView)
{
	if (nullptr == m_glProgram){
		G::log("invalid glProgram !");
		return;
	}

	if (m_uniformAttributeValueDirty){
		for (auto &uniform : m_uniformsByName){
			m_uniformsById[uniform.second].m_uniform = m_glProgram->getUniform(uniform.first);
		}

		m_attributeFlags = 0;
		for (auto attribute : m_attributes){
			attribute.second.m_attribute = m_glProgram->getAttribute(attribute.first);
			if (attribute.second.m_enabled){
				m_attributeFlags |= 1 << attribute.second.m_attribute->location;
			}
		}
		m_uniformAttributeValueDirty = false;
	}

	// use
	m_glProgram->use();
}

void GLProgramState::applyAttributes(bool applyAttribFlags/* = true*/)
{
	if (m_attributeFlags){

		/**enable/disable vertex attribs*/
		if (applyAttribFlags){
			G::enableVertexAttributes(m_attributeFlags);
		}

		/**set attribute*/
		for (auto &attri : m_attributes){
			attri.second.apply();
		}
	}
}

void GLProgramState::applyUniforms()
{
	for (auto &uniform : m_uniformsById){
		uniform.second.apply();
	}
}


void GLProgramState::setGLProgram(GLProgram* glProgram)
{
	if (nullptr == m_glProgram){
		G::log("Error : GLProgramState::setGLProgram invalid program !");
		return;
	}

	if (m_glProgram != glProgram){
		resetGLProgram();
		init(glProgram);
	}
}

void GLProgramState::setAttributeCallback(const std::string& name, const std::function<void(Attribute*)> &callback)
{
	AttributeValue *av = getAttributeValue(name);
	if (av){
		av->setCallback(callback);
		m_attributeFlags |= 1 << av->m_attribute->location;
	}
	else{
		G::log("Warning : Attribute not found: %s", name.c_str());
	}
}

void GLProgramState::setAttributePointer(const std::string& name, GLint size, GLenum type, GLboolean normalized, GLsizei stripe, GLvoid* pointer)
{
	AttributeValue *av = getAttributeValue(name);
	if (av){
		av->setPointer(size, type, normalized, stripe, pointer);
		m_attributeFlags |= 1 << av->m_attribute->location;
	}
	else{
		G::log("Warning : Attribute not found: %s", name.c_str());
	}
}

AttributeValue* GLProgramState::getAttributeValue(const std::string& name)
{
	const auto iter = m_attributes.find(name);
	if (iter != m_attributes.end()){
		return &iter->second;
	}

	return nullptr;
}

UniformValue* GLProgramState::getUniformValue(const std::string& name)
{
	const auto iter = m_uniformsByName.find(name);
	if (iter != m_uniformsByName.end())
	{
		return &m_uniformsById[iter->second];
	}

	return nullptr;
}

UniformValue* GLProgramState::getUniformValue(GLint location)
{
	const auto iter = m_uniformsById.find(location);
	if (iter != m_uniformsById.end()){
		return &iter->second;
	}

	return nullptr;
}

//  [11/28/2017 Administrator]
void GLProgramState::setUniformInt(const std::string& name, int value)
{
	auto v = getUniformValue(name);
	if (v){
		v->setInt(value);
	}
	else
	{
		G::log("warning : Uniform not found: %s", name.c_str());
	}
}

void GLProgramState::setUniformFloat(const std::string& name, float value)
{
	auto v = getUniformValue(name);
	if (v){
		v->setFloat(value);
	}
	else
	{
		G::log("warning : Uniform not found: %s", name.c_str());
	}
}

void GLProgramState::setUniformVec2(const std::string& name, const Vec2& value)
{
	auto v = getUniformValue(name);
	if (v){
		v->setVec2(value);
	}
	else
	{
		G::log("warning : Uniform not found: %s", name.c_str());
	}
}

void GLProgramState::setUniformVec3(const std::string& name, const Vec3& value)
{
	auto v = getUniformValue(name);
	if (v){
		v->setVec3(value);
	}
	else
	{
		G::log("warning : Uniform not found: %s", name.c_str());
	}
}

void GLProgramState::setUniformVec4(const std::string& name, const Vec4& value)
{
	auto v = getUniformValue(name);
	if (v){
		v->setVec4(value);
	}
	else
	{
		G::log("warning : Uniform not found: %s", name.c_str());
	}
}

void GLProgramState::setUniformMat4(const std::string& name, const Mat4& value)
{
	auto v = getUniformValue(name);
	if (v){
		v->setMat4(value);
	}
	else
	{
		G::log("warning : Uniform not found: %s", name.c_str());
	}
}

void GLProgramState::setUniformTexture(const std::string& name, Texture2D* texture)
{
	if (nullptr == texture){
		G::log("warning : GLProgramState::setUniformTexture invalid texture !");
		return;
	}
	setUniformTexture(name, texture->getName());
}

void GLProgramState::setUniformTexture(const std::string& name, GLuint textureid)
{
	auto v = getUniformValue(name);
	if (v){
		if (m_boundTextureUnits.find(name) != m_boundTextureUnits.end()){
			v->setTexture(textureid, m_boundTextureUnits[name]);
		}
		else
		{
			v->setTexture(textureid, m_textureUnitIndex);
			m_boundTextureUnits[name] = m_textureUnitIndex++;
		}
	}
	else{
		G::log("warning: Uniform not found: %s", name.c_str()); 
	}
}

void GLProgramState::setUniformCallback(const std::string& name, const std::function<void(GLProgram*, Uniform*)>& callback)
{
	auto v = getUniformValue(name);
	if (v){
		v->setCallback(callback);
	}
	else{
		G::log("warning: Uniform not found: %s", name.c_str());
	}
}

//  [11/28/2017 Administrator]
void GLProgramState::setUniformInt(GLint location, int value)
{
	auto v = getUniformValue(location);
	if (v){
		v->setInt(value);
	}
	else{
		G::log("warning : Uniform at location not found: %i", location);
	}
}

void GLProgramState::setUniformFloat(GLint location, float value)
{
	auto v = getUniformValue(location);
	if (v){
		v->setFloat(value);
	}
	else{
		G::log("warning : Uniform at location not found: %i", location);
	}
}

void GLProgramState::setUniformVec2(GLint location, const Vec2& value)
{
	auto v = getUniformValue(location);
	if (v){
		v->setVec2(value);
	}
	else{
		G::log("warning : Uniform at location not found: %i", location);
	}
}

void GLProgramState::setUniformVec3(GLint location, const Vec3& value)
{
	auto v = getUniformValue(location);
	if (v){
		v->setVec3(value);
	}
	else{
		G::log("warning : Uniform at location not found: %i", location);
	}
}

void GLProgramState::setUniformVec4(GLint location, const Vec4& value)
{
	auto v = getUniformValue(location);
	if (v){
		v->setVec4(value);
	}
	else{
		G::log("warning : Uniform at location not found: %i", location);
	}
}

void GLProgramState::setUniformMat4(GLint location, const Mat4& value)
{
	auto v = getUniformValue(location);
	if (v){
		v->setMat4(value);
	}
	else{
		G::log("warning : Uniform at location not found: %i", location);
	}
}

void GLProgramState::setUniformTexture(GLint location, Texture2D* texture)
{
	if (nullptr == texture){
		G::log("warning : GLProgramState::setUniformTexture invalid texture.");
		return;
	}

	setUniformTexture(location, texture->getName());
}

void GLProgramState::setUniformTexture(GLint location, GLuint textureid)
{
	auto v = getUniformValue(location);
	if (v){
		if (m_boundTextureUnits.find(v->m_uniform->name) != m_boundTextureUnits.end()){
			v->setTexture(textureid, m_boundTextureUnits[v->m_uniform->name]);
		}
		else
		{
			v->setTexture(textureid, m_textureUnitIndex);
			m_boundTextureUnits[v->m_uniform->name] = m_textureUnitIndex++;
		}
	}
	else{
		G::log("warning: Uniform at location not found: %i", location);
	}
}

void GLProgramState::setUniformCallback(GLint location, const std::function<void(GLProgram*, Uniform*)>& callback)
{
	auto v = getUniformValue(location);
	if (v){
		v->setCallback(callback);
	}
	else{
		G::log("warning: Uniform at location not found: %i", location);
	}
}