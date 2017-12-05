/************************************************************************/
/* 
* Program state
*/
/************************************************************************/

#ifndef  __GLPROGRAMSTATE_H__
#define  __GLPROGRAMSTATE_H__

#include <cstdint>
#include <unordered_map>

#include "../Comm/Macors.h"
#include "../Comm/Math.h"
#include "Ref.h"
#include "../GDefine.h"

NAMESPACE_BEGIN

class GLProgram;
struct Uniform;
struct Attribute;

class Texture2D;

/**
* uniform values
*/
class UniformValue
{
	friend class GLProgram;
	friend class GLProgramState;

public:
	UniformValue();
	UniformValue(Uniform* uniform, GLProgram* glProgram);
	virtual ~UniformValue();

	void setInt(int val);
	void setFloat(int val);
	void setVec2(const Vec2& val);
	void setVec3(const Vec3& val);
	void setVec4(const Vec4& val);
	void setMat4(const Mat4& mat);
	void setTexture(GLuint textureId, GLuint textureUint);

	void setCallback(const std::function<void(GLProgram*, Uniform*)> &callback);

	// apply the uniform's value
	void apply();

protected:
	Uniform *m_uniform;
	GLProgram *m_glProgram;

	bool m_useCallback;

	union U
	{
		int		intVaule;
		float	floatVaule;
		float	v2Value[2];
		float	v3Value[3];
		float	v4Value[4];
		float	mat4Value[16];

		struct{
			GLuint textureId;		// texture id
			GLuint textureUnit;		// texture bind id
		}m_tex;

		std::function<void(GLProgram*, Uniform*)> *callback;

		U(){ memset(this, 0, sizeof(*this)); }
		~U(){}
		U& operator = (const U& other){
			memcpy(this, &other, sizeof(*this));
			return *this;
		}
	}m_value;
};


class AttributeValue
{
	friend class GLProgram;
	friend class GLProgramState;

public:
	AttributeValue();
	AttributeValue(Attribute *attribute);
	virtual ~AttributeValue();

	void setPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer);
	void setCallback(const std::function<void(Attribute*)> &callback);

	void apply();

protected:

	Attribute *m_attribute;

	bool m_enabled;
	bool m_useCallback;

	union U
	{
		struct  
		{
			GLint size;
			GLenum type;
			GLboolean normalized;
			GLsizei stride;
			GLvoid *pointer;
		}ptr;

		std::function<void(Attribute*)> *callback;

		U(){ memset(this, 0, sizeof(*this)); }
		~U(){}

		U& operator=(const U& other){
			memcpy(this, &other, sizeof(*this));
			return *this;
		}
	}m_value;
};


class GLProgramState : public Ref
{
	friend class GLProgramStateCache;
public:

	/** returns a new instance of GLProgramState for a given GLProgram */
	static GLProgramState* create(GLProgram* glProgram);

	/** gets-or-creates an instance of GLProgramState for a given GLProgram */
	static GLProgramState* getOrCreateWithGLProgram(GLProgram* glProgram);

	static GLProgramState* getOrCreateWithGLProgramName(const std::string &glProgramName);

	/**apply GLProgram, attributes and uniforms*/
	void apply(const Mat4& modelView);

	/**copy the glprogram's uniform && attribute to glProgramState*/
	void applyGLProgram(const Mat4& movelView);

	/**apply attributes */
	void applyAttributes(bool applyAttribFlags = true);

	/**apply uniforms*/
	void applyUniforms();

	
	void setGLProgram(GLProgram* glProgram);
	GLProgram* getGLProgram() const { return m_glProgram; }

	/**vertex attribute*/
	uint32_t getAttributeFlags() const { return m_attributeFlags; }
	unsigned int getAttributeCount() const { return m_attributes.size(); }

	void setAttributeCallback(const std::string& name, const std::function<void(Attribute*)> &callback);
	void setAttributePointer(const std::string& name, GLint size, GLenum type, GLboolean normalized, GLsizei stripe, GLvoid* pointer);

	/**user defined uniforms*/
	unsigned int getUniformCount() const { return m_uniformsById.size(); }

	void setUniformInt(const std::string& name, int value);
	void setUniformFloat(const std::string& name, float value);
	void setUniformVec2(const std::string& name, const Vec2& value);
	void setUniformVec3(const std::string& name, const Vec3& value);
	void setUniformVec4(const std::string& name, const Vec4& value);
	void setUniformMat4(const std::string& name, const Mat4& value);
	void setUniformTexture(const std::string& name, Texture2D* texture);
	void setUniformTexture(const std::string& name, GLuint textureid);
	void setUniformCallback(const std::string& name, const std::function<void(GLProgram*, Uniform*)>& callback);

	void setUniformInt(GLint location, int value);
	void setUniformFloat(GLint location, float value);
	void setUniformVec2(GLint location, const Vec2& value);
	void setUniformVec3(GLint location, const Vec3& value);
	void setUniformVec4(GLint location, const Vec4& value);
	void setUniformMat4(GLint location, const Mat4& value);
	void setUniformTexture(GLint location, Texture2D* texture);
	void setUniformTexture(GLint location, GLuint textureid);
	void setUniformCallback(GLint location, const std::function<void(GLProgram*, Uniform*)>& callback);

	GLProgramState();
	~GLProgramState();

protected:
	bool init(GLProgram* program);
	void resetGLProgram();

	AttributeValue* getAttributeValue(const std::string& name);
	UniformValue* getUniformValue(const std::string& name);
	UniformValue* getUniformValue(GLint location);


protected:
	GLProgram *m_glProgram;

	std::unordered_map<std::string, GLint> m_uniformsByName;
	std::unordered_map<GLint, UniformValue> m_uniformsById;
	std::unordered_map<std::string, AttributeValue> m_attributes;
	std::unordered_map<std::string, int> m_boundTextureUnits;


	bool m_uniformAttributeValueDirty;
	int m_textureUnitIndex;
	uint32_t m_attributeFlags;


};

NAMESPACE_END

#endif