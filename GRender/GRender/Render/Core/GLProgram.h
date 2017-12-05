/************************************************************************/
/* 
* GLProgram 管理类
*/
/************************************************************************/

#ifndef  __GLPROGRAM_H__
#define  __GLPROGRAM_H__

#include "../Comm/Macors.h"

#include "Ref.h"
#include "../GDefine.h"

#include <unordered_map>

NAMESPACE_BEGIN

/**
* 顶点属性结构
*/
struct Attribute
{
	GLuint location;		// 索引位置
	GLint size;				// Attribute数位(FLOAT_32 * 2 or 3 ...)
	GLenum type;			// type (FLOAT_32, UNSIGNED_SHORT ...)
	std::string name;		// name 
};

/**
* shader全局数据结构
*/
struct Uniform
{
	GLint location;
	GLint size;
	GLenum type;
	std::string name;
};

class GLProgram : public Ref
{
public:

	enum 
	{
		VERTEX_ATTRIBUTE_POSITION,
		VERTEX_ATTRIBUTE_NORMAL,
		VERTEX_ATTRIBUTE_COLOR,
		VERTEX_ATTRIBUTE_TEXTURE0,
		VERTEX_ATTRIBUTE_TEXTURE1,
		VERTEX_ATTRIBUTE_TEXTURE2,
		VERTEX_ATTRIBUTE_TEXTURE3
	};

	enum
	{
		UNIFORM_AMBIENT_COLOR,
		UNIFORM_P_MATRIX,
		UNIFORM_MV_MATRIX,
		UNIFORM_MVP_MATRIX,
		UNIFORM_NORMAL_MATRIX,
		UNIFORM_SAMPLER0,
		UNIFORM_SAMPLER1,
		UNIFORM_SAMPLER2,
		UNIFORM_SAMPLER3,
		UNIFORM_DISPLAYCOLOR,
		UNIFORM_MAX
	};

	// shader names
	static const char* SHADER_NAME_POSITION;
	static const char* SHADER_NAME_POSITION_COLOR;
	static const char* SHADER_NAME_POSITION_TEXTURE_COLOR;
	static const char* SHADER_NAME_POSITION_NORMAL_COLOR;
	static const char* SHADER_NAME_POSITION_NORMAL_TEXTURE_COLOR;

	// attribute names
	static const char* ATTRIBUTE_NAME_POSITION;
	static const char* ATTRIBUTE_NAME_COLOR;
	static const char* ATTRIBUTE_NAME_NORMAL;
	static const char* ATTRIBUTE_NAME_TEXTURE0;
	static const char* ATTRIBUTE_NAME_TEXTURE1;
	static const char* ATTRIBUTE_NAME_TEXTURE2;
	static const char* ATTRIBUTE_NAME_TEXTURE3;

	// uniform names
	static const char* UNIFORM_NAME_AMBIENT_COLOR;
	static const char* UNIFROM_NAME_DIFFUSE_COLOR;
	static const char* UNIFROM_NAME_SPECULAR_COLOR;
	static const char* UNIFORM_NAME_SHININESS;
	static const char* UNIFORM_NAME_P_MATRIX;
	static const char* UNIFORM_NAME_MV_MATRIX;
	static const char* UNIFORM_NAME_MVP_MATRIX;
	static const char* UNIFORM_NAME_NORMAL_MATRIX;
	static const char* UNIFORM_NAME_SAMPLER0;
	static const char* UNIFORM_NAME_SAMPLER1;
	static const char* UNIFORM_NAME_SAMPLER2;
	static const char* UNIFORM_NAME_SAMPLER3;
	static const char* UNIFORM_NAME_DISPLAYCOLOR;
	static const char* UNIFORM_NAME_LIGHTPOSITION;
	static const char* UNIFORM_NAME_LIGHTCOLOR;

public:
	// constructor
	GLProgram();
	virtual ~GLProgram();

	/** 
	* Initializes the GLProgram with a vertex and fragment with contents of filenames
	* notices: the vShader && fShader is the full path file.
	*/
	static GLProgram* createProgramWithFile(const std::string& vShader, const std::string& fShader);
	bool initProgramWithFile(const std::string& vShader, const std::string& fShader);

	/**
	* Get Attribute / Uniform
	*/
	Attribute* getAttribute(const std::string& name);
	Uniform* getUniform(const std::string& name);

	/**
	* It will add a new attribute to the shader by calling glBindAttribLocation
	*/
	void bindAttributeLocation(const std::string& attributeName, GLuint location) const;

	/**
	* calls glGetAttribLocation
	*/
	GLint getAttributeLocation(const std::string& attributeName) const;

	/**
	* calls glGetUniformLocation()
	*/
	GLint getUniformLocation(const std::string& uniformName) const;

	/**
	* links the glProgram
	*/
	bool link();

	/**
	* it will call glUseProgram()
	*/
	void use();

	/**
	* It will create 4 uniforms:
	- kUniformPMatrix
	- kUniformMVMatrix
	- kUniformMVPMatrix
	- GLProgram::UNIFORM_SAMPLER

	* And it will bind "GLProgram::UNIFORM_SAMPLER" to 0
	*/
	void updateUniforms();


	/**
	*calls glUniform XX i only if the values are different than the previous call for this same shader program.
	*
	*/
	void setUniformLocationWith1i(GLint location, GLint i1);
	void setUniformLocationWith2i(GLint location, GLint i1, GLint i2);
	void setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3);
	void setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4);

	/** 
	*calls glUniformXiv only if the values are different than the previous call for this same shader program. 
	*
	*/
	void setUniformLocationWith2iv(GLint location, GLint *ints, unsigned int numberOfArrays);
	void setUniformLocationWith3iv(GLint location, GLint *ints, unsigned int numberOfArrays);
	void setUniformLocationWith4iv(GLint location, GLint *ints, unsigned int numberOfArrays);

	/**
	*calls glUniform XX f only if the values are different than the previous call for this same shader program.
	*
	*/
	void setUniformLocationWith1f(GLint location, GLfloat i1);
	void setUniformLocationWith2f(GLint location, GLfloat i1, GLfloat i2);
	void setUniformLocationWith3f(GLint location, GLfloat i1, GLfloat i2, GLfloat i3);
	void setUniformLocationWith4f(GLint location, GLfloat i1, GLfloat i2, GLfloat i3, GLfloat i4);

	/**
	*calls glUniformXfv only if the values are different than the previous call for this same shader program.
	*
	*/
	void setUniformLocationWith1fv(GLint location, GLfloat *floats, unsigned int numberOfArrays);
	void setUniformLocationWith2fv(GLint location, GLfloat *floats, unsigned int numberOfArrays);
	void setUniformLocationWith3fv(GLint location, GLfloat *floats, unsigned int numberOfArrays);
	void setUniformLocationWith4fv(GLint location, GLfloat *floats, unsigned int numberOfArrays);

	/** calls glUniformMatrixXfv only if the values are different than the previous call for this same shader program. */
	void setUniformLocationWithMatrix2fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices);
	void setUniformLocationWithMatrix3fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices);
	void setUniformLocationWithMatrix4fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices);


	/** Get Program */
	inline const GLuint getProgram() const { return m_program; }


	/** will update the builtin uniforms if they are different than the previous call for this same shader program. */
	void setUniformsForBuiltins();
	void setUniformsForBuiltins(const Mat4 &modelView);

	/** Reset */
	void reset();

protected:
	/** Compile raw shader */
	bool compileShader(GLuint* shader, GLenum type, const GLchar* source);

	/** parse shaders*/
	void parseAttributes();
	void parseUniforms();

	/** cache uniform data to judge if need to update*/
	bool needUpdateUniformLoaction(GLint location, const GLvoid* data, unsigned int bytes);


	std::string loadShaderFromFile(const std::string& filename);

protected:
	
	GLuint m_program;
	GLuint m_vertShader;
	GLuint m_fragShader;

	// ##inner get uniforms location
	GLint  m_uniformsLocation[UNIFORM_MAX];

	bool m_hasShaderCompiler;

	std::unordered_map<std::string, Uniform> m_uniforms;
	std::unordered_map<std::string, Attribute> m_attributes;
	std::unordered_map<GLint, GLvoid*> m_hashForUniforms;

	struct flag_struct
	{
		unsigned int useP	: 1;
		unsigned int useMV : 1;
		unsigned int useMVP : 1;
		unsigned int useNormal : 1;
		unsigned int useDisplayColor : 1;

		flag_struct(){ memset(this, 0, sizeof(*this)); }
	}m_flags;


	friend class GLProgramState;
};

NAMESPACE_END


#endif