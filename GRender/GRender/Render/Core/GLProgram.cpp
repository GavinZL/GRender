#include "GL/glew.h"
#include "GLProgram.h"

#include "../Comm/Utils.h"

#include "Engine.h"

#include <fstream>

USING_NAMESPACE_G

#ifdef _DEBUG

#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            G::log("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
										     } \
					} while (false);

#else

#define CHECK_GL_ERROR_DEBUG() ;

#endif // _DEBUG

// shader names
const char* GLProgram::SHADER_NAME_POSITION = "shader_position";
const char* GLProgram::SHADER_NAME_POSITION_COLOR = "shader_position_color";
const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR = "shader_position_texture_color";
const char* GLProgram::SHADER_NAME_POSITION_NORMAL_COLOR = "shader_position_normal_color";
const char* GLProgram::SHADER_NAME_POSITION_NORMAL_TEXTURE = "shader_position_normal_texture";
const char* GLProgram::SHADER_NAME_POSITION_NORMAL_TEXTURE_COLOR = "shader_position_normal_color_texture";

// attribute names
const char* GLProgram::ATTRIBUTE_NAME_POSITION = "G_position";
const char* GLProgram::ATTRIBUTE_NAME_COLOR = "G_color";
const char* GLProgram::ATTRIBUTE_NAME_NORMAL = "G_normal";
const char* GLProgram::ATTRIBUTE_NAME_TEXTURE0 = "G_texCoord0";
const char* GLProgram::ATTRIBUTE_NAME_TEXTURE1 = "G_texCoord1";
const char* GLProgram::ATTRIBUTE_NAME_TEXTURE2 = "G_texCoord2";
const char* GLProgram::ATTRIBUTE_NAME_TEXTURE3 = "G_texCoord3";

// uniform names
const char* GLProgram::UNIFORM_NAME_AMBIENT_COLOR = "G_ambientColor";
const char* GLProgram::UNIFROM_NAME_DIFFUSE_COLOR = "G_diffuseColor";
const char* GLProgram::UNIFROM_NAME_SPECULAR_COLOR = "G_specularColor";
const char* GLProgram::UNIFORM_NAME_SHININESS = "G_shininess";

const char* GLProgram::UNIFORM_NAME_P_MATRIX = "G_pMatrix";
const char* GLProgram::UNIFORM_NAME_MV_MATRIX = "G_mvMatrix";
const char* GLProgram::UNIFORM_NAME_MVP_MATRIX = "G_mvpMatrix";
const char* GLProgram::UNIFORM_NAME_NORMAL_MATRIX = "G_normalMatrix";
const char* GLProgram::UNIFORM_NAME_SAMPLER0 = "G_texture0";
const char* GLProgram::UNIFORM_NAME_SAMPLER1 = "G_texture1";
const char* GLProgram::UNIFORM_NAME_SAMPLER2 = "G_texture2";
const char* GLProgram::UNIFORM_NAME_SAMPLER3 = "G_texture3";

const char* GLProgram::UNIFORM_NAME_DISPLAYCOLOR = "G_displayColor";
const char* GLProgram::UNIFORM_NAME_LIGHTPOSITION = "G_lightPosition";
const char* GLProgram::UNIFORM_NAME_LIGHTCOLOR = "G_lightColor";



GLProgram::GLProgram()
	: m_program(0)
	, m_vertShader(0)
	, m_fragShader(0)
{
	memset(m_uniformsLocation, 0, sizeof(m_uniformsLocation));
}

GLProgram::~GLProgram()
{
	G::log("%s %d deallocing GLProgram : %p ", __FUNCTION__, __LINE__, this);


	if (m_vertShader){
		glDeleteShader(m_vertShader);
	}

	if (m_fragShader){
		glDeleteShader(m_fragShader);
	}

	m_vertShader = m_fragShader = 0;

	if (m_program){
		glDeleteProgram(m_program);
	}

	for (auto e : m_hashForUniforms){
		free(e.second);
	}

	m_hashForUniforms.clear();
}

GLProgram* GLProgram::createProgramWithFile(const std::string& vShader, const std::string& fShader)
{
	auto _program = new (std::nothrow) GLProgram;
	if (_program && _program->initProgramWithFile(vShader, fShader)){
		_program->link();
		_program->updateUniforms();
	}

	return _program;
}

std::string GLProgram::loadShaderFromFile(const std::string& filename)
{
	if (filename.empty())
		return "";

	std::string source;
	std::ifstream stream(filename, std::ios::in);

	if (stream.is_open())
	{
		std::string line = "";
		while (getline(stream, line))
		{
			source += "\n" + line;
		}
		stream.close();
	}
	else
	{
		G::log("can't open shader file : %s .", filename.c_str());
		return 0;
	}

	return source;
	/*
	unsigned char* buffer = nullptr;

	do{
		FILE *fp = fopen(filename.c_str(), "rb");
		if (!fp){
			break;
		}

		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		
		buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);

		long readsize = fread(buffer, sizeof(unsigned char), size, fp);
		fclose(fp);

		if (readsize < size){
			buffer[readsize] = '\0';
		}
	} while (0);
	
	return std::string((char*)buffer);*/
}

bool GLProgram::initProgramWithFile(const std::string& vShader, const std::string& fShader)
{
	if (vShader.empty() || fShader.empty()){
		G::log("Warning : shader file invalid .\n");
		return false;
	}

	// 1.
	m_program = glCreateProgram();
	CHECK_GL_ERROR_DEBUG();


	const char* vendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	
	//G::log("Vendor : %s \n", vendorName);
	//G::log("Version : %s\n", version);

	// 2.
	m_vertShader = m_fragShader = 0;

	// 3. 
	std::string vSource = loadShaderFromFile(vShader);
	std::string fSource = loadShaderFromFile(fShader);

	//G::log("%s", vSource.c_str());
	//G::log("\n------------------------------------------------\n");
	//G::log("%s", fSource.c_str());

	if (!vShader.empty()){
		if (!compileShader(&m_vertShader, GL_VERTEX_SHADER, vSource.c_str())){
			G::log("ERROR: Failed to compile vertex shader");
			return false;
		}
	}

	if (!fShader.empty()){
		if (!compileShader(&m_fragShader, GL_FRAGMENT_SHADER, fSource.c_str())){
			G::log("ERROR: Failed to compile fragment shader");
			return false;
		}
	}

	// 3.
	if (m_vertShader){
		glAttachShader(m_program, m_vertShader);
	}
	CHECK_GL_ERROR_DEBUG();

	if (m_fragShader){
		glAttachShader(m_program, m_fragShader);
	}
	CHECK_GL_ERROR_DEBUG();


	m_hashForUniforms.clear();
	return true;
}

bool GLProgram::compileShader(GLuint* shader, GLenum type, const GLchar* source)
{
	GLint status = GL_FALSE;

	if (!source){
		G::log("Error : GLProgram::compileShader : shader source is null.");
		return false;
	}

	// compile
	*shader = glCreateShader(type);
	glShaderSource(*shader, /*sizeof(source) / sizeof(*source)*/1, &source, nullptr);
	glCompileShader(*shader);

	// status
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
	//if (!status){
	//	
	//	GLsizei length;
	//	glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &length);
	//	GLchar* src = (GLchar*)malloc(sizeof(GLchar) * length);

	//	glGetShaderSource(*shader, length, nullptr, src);
	//	G::log("\nERROR: Failed to compile shader:\n%s", src);

	//	free(src);

	//	return false;
	//}

	int infoLength = 0;
	glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &infoLength);
	if (infoLength > 0){

		std::vector<char> vsErrorMsg(infoLength + 1);
		glGetShaderInfoLog(*shader, infoLength, nullptr, &vsErrorMsg[0]);
		G::log("ERROR: %s \n", &vsErrorMsg[0]);
	}
	

	return (status == GL_TRUE);
}

bool GLProgram::link()
{
	if (!m_program){
		G::log("Cannot link invalid program !");
		return false;
	}

	GLint status = GL_TRUE;

	// ##指定绑定索引
	static const struct{
		const char* attributeName;
		int location;
	} attribute_location[] =
	{
		{GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIBUTE_POSITION},
		{GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIBUTE_COLOR},
		{GLProgram::ATTRIBUTE_NAME_NORMAL, GLProgram::VERTEX_ATTRIBUTE_NORMAL},
		{GLProgram::ATTRIBUTE_NAME_TEXTURE0, GLProgram::VERTEX_ATTRIBUTE_TEXTURE0},
		{GLProgram::ATTRIBUTE_NAME_TEXTURE1, GLProgram::VERTEX_ATTRIBUTE_TEXTURE1},
		{GLProgram::ATTRIBUTE_NAME_TEXTURE2, GLProgram::VERTEX_ATTRIBUTE_TEXTURE2},
		{GLProgram::ATTRIBUTE_NAME_TEXTURE3, GLProgram::VERTEX_ATTRIBUTE_TEXTURE3}
	};

	const int size = sizeof(attribute_location) / sizeof(attribute_location[0]);

	for (int i = 0; i < size; ++i){
		glBindAttribLocation(m_program, attribute_location[i].location, attribute_location[i].attributeName);
	}

	// ##link
	glLinkProgram(m_program);

	// ##parse
	parseAttributes();
	parseUniforms();

	// ##delete
	if (m_vertShader){
		glDeleteShader(m_vertShader);
	}

	if (m_fragShader){
		glDeleteShader(m_fragShader);
	}

	m_vertShader = m_fragShader = 0;

	return (status == GL_TRUE);
}

void GLProgram::use()
{
	glUseProgram(m_program);
}

void GLProgram::parseAttributes()
{
	m_attributes.clear();

	/**Query and store vertex attribute meta-data from the program.*/
	GLint activeAttribs = 0;
	GLint length = 0;

	glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
	if (activeAttribs > 0){

		Attribute attribute;
		glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length);
		if (length > 0){

			GLchar* attributeName = (GLchar*)alloca(length + 1);
			for (int i = 0; i < activeAttribs; ++i){

				/**Query attribute info*/
				glGetActiveAttrib(m_program, i, length, nullptr, &attribute.size, &attribute.type, attributeName);
				attributeName[length] = '\0';
				attribute.name = std::string(attributeName);

				attribute.location = glGetAttribLocation(m_program, attributeName);
				m_attributes[attribute.name] = attribute;
			}
		}
	}
	else
	{
		GLchar error[1024] = { 0 };
		glGetProgramInfoLog(m_program, sizeof(error), nullptr, error);
		G::log("Error linking shader program: '%s'\n", error);
	}

}

void GLProgram::parseUniforms()
{
	m_uniforms.clear();

	/**Query and store uniforms from the program.*/
	GLint activeUniforms = 0;
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &activeUniforms);
	if (activeUniforms > 0){

		GLint length;
		glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length);
		if (length > 0){

			Uniform uniform;
			GLchar *uniformName = (GLchar*)alloca(length + 1);

			for (int i = 0; i < activeUniforms; ++i){
				glGetActiveUniform(m_program, i, length, nullptr, &uniform.size, &uniform.type, uniformName);
				uniformName[length] = '\0';

				/**remove possible array '[]' from uniform name*/
				if (length > 3){
					char *c = strrchr(uniformName, '[');
					if (c){
						*c = '\0';
					}
				}

				uniform.name = std::string(uniformName);
				uniform.location = glGetUniformLocation(m_program, uniformName);

				CHECK_GL_ERROR_DEBUG();

				m_uniforms[uniform.name] = uniform;
			}
		}
	}
	else
	{
		GLchar error[1024] = { 0 };
		glGetProgramInfoLog(m_program, sizeof(error), nullptr, error);
		G::log("Error linking shader program: '%s'\n", error);
	}

}

void GLProgram::updateUniforms()
{
	m_uniformsLocation[UNIFORM_AMBIENT_COLOR] = glGetUniformLocation(m_program, UNIFORM_NAME_AMBIENT_COLOR);
	m_uniformsLocation[UNIFORM_P_MATRIX] = glGetUniformLocation(m_program, UNIFORM_NAME_P_MATRIX);
	m_uniformsLocation[UNIFORM_MV_MATRIX] = glGetUniformLocation(m_program, UNIFORM_NAME_MV_MATRIX);
	m_uniformsLocation[UNIFORM_MVP_MATRIX] = glGetUniformLocation(m_program, UNIFORM_NAME_MVP_MATRIX);
	m_uniformsLocation[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(m_program, UNIFORM_NAME_NORMAL_MATRIX);
	m_uniformsLocation[UNIFORM_SAMPLER0] = glGetUniformLocation(m_program, UNIFORM_NAME_SAMPLER0);
	m_uniformsLocation[UNIFORM_SAMPLER1] = glGetUniformLocation(m_program, UNIFORM_NAME_SAMPLER1);
	m_uniformsLocation[UNIFORM_SAMPLER2] = glGetUniformLocation(m_program, UNIFORM_NAME_SAMPLER2);
	m_uniformsLocation[UNIFORM_SAMPLER3] = glGetUniformLocation(m_program, UNIFORM_NAME_SAMPLER3);
	m_uniformsLocation[UNIFORM_DISPLAYCOLOR] = glGetUniformLocation(m_program, UNIFORM_NAME_DISPLAYCOLOR);

	// flags
	m_flags.useP = m_uniformsLocation[UNIFORM_P_MATRIX] != -1;
	m_flags.useMV = m_uniformsLocation[UNIFORM_MV_MATRIX] != -1;
	m_flags.useMVP = m_uniformsLocation[UNIFORM_MVP_MATRIX] != -1;
	m_flags.useNormal = m_uniformsLocation[UNIFORM_NORMAL_MATRIX] != -1;
	m_flags.useDisplayColor = m_uniformsLocation[UNIFORM_DISPLAYCOLOR] != -1;

	this->use();

	if (m_uniformsLocation[UNIFORM_SAMPLER0] != -1){
		setUniformLocationWith1i(m_uniformsLocation[UNIFORM_SAMPLER0], 0);
	}
	if (m_uniformsLocation[UNIFORM_SAMPLER1] != -1){
		setUniformLocationWith1i(m_uniformsLocation[UNIFORM_SAMPLER1], 1);
	}
	if (m_uniformsLocation[UNIFORM_SAMPLER2] != -1){
		setUniformLocationWith1i(m_uniformsLocation[UNIFORM_SAMPLER2], 2);
	}
	if (m_uniformsLocation[UNIFORM_SAMPLER3] != -1){
		setUniformLocationWith1i(m_uniformsLocation[UNIFORM_SAMPLER3], 3);
	}
}

Attribute* GLProgram::getAttribute(const std::string& name)
{
	const auto iter = m_attributes.find(name);
	if (iter != m_attributes.end()){
		return &iter->second;
	}

	return nullptr;
}

Uniform* GLProgram::getUniform(const std::string& name)
{
	const auto iter = m_uniforms.find(name);
	if (iter != m_uniforms.end()){
		return &iter->second;
	}

	return nullptr;
}

void GLProgram::bindAttributeLocation(const std::string& attributeName, GLuint location) const
{
	glBindAttribLocation(m_program, location, attributeName.c_str());
}

GLint GLProgram::getAttributeLocation(const std::string& attributeName) const
{
	return glGetAttribLocation(m_program, attributeName.c_str());
}

GLint GLProgram::getUniformLocation(const std::string& uniformName) const
{
	return glGetUniformLocation(m_program, uniformName.c_str());
}

void GLProgram::setUniformLocationWith1i(GLint location, GLint i1)
{
	bool updated = needUpdateUniformLoaction(location, &i1, sizeof(i1) * 1);
	if (updated){
		glUniform1i(location, i1);
	}
}

void GLProgram::setUniformLocationWith2i(GLint location, GLint i1, GLint i2)
{
	GLint ints[2] = { i1, i2 };
	bool updated = needUpdateUniformLoaction(location, ints, sizeof(ints));
	if (updated){
		glUniform2i(location, i1, i2);
	}
}

void GLProgram::setUniformLocationWith3i(GLint location, GLint i1, GLint i2, GLint i3)
{
	GLint ints[3] = { i1, i2, i3 };
	bool updated = needUpdateUniformLoaction(location, ints, sizeof(ints));
	if (updated){
		glUniform3i(location, i1, i2, i3);
	}
}

void GLProgram::setUniformLocationWith4i(GLint location, GLint i1, GLint i2, GLint i3, GLint i4)
{
	GLint ints[4] = { i1, i2, i3 ,i4 };
	bool updated = needUpdateUniformLoaction(location, ints, sizeof(ints));
	if (updated){
		glUniform4i(location, i1, i2, i3, i4);
	}
}

void GLProgram::setUniformLocationWith2iv(GLint location, GLint *ints, unsigned int numberOfArrays)
{
	bool updated = needUpdateUniformLoaction(location, ints, sizeof(int) * 2 * numberOfArrays);
	if (updated){
		glUniform2iv(location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith3iv(GLint location, GLint *ints, unsigned int numberOfArrays)
{
	bool updated = needUpdateUniformLoaction(location, ints, sizeof(int) * 3 * numberOfArrays);
	if (updated){
		glUniform3iv(location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith4iv(GLint location, GLint *ints, unsigned int numberOfArrays)
{
	bool updated = needUpdateUniformLoaction(location, ints, sizeof(int) * 4 * numberOfArrays);
	if (updated){
		glUniform4iv(location, (GLsizei)numberOfArrays, ints);
	}
}

void GLProgram::setUniformLocationWith1f(GLint location, GLfloat i1)
{
	bool updated = needUpdateUniformLoaction(location, &i1, sizeof(i1) * 1);
	if (updated){
		glUniform1f(location, i1);
	}
}

void GLProgram::setUniformLocationWith2f(GLint location, GLfloat i1, GLfloat i2)
{
	GLfloat floats[2] = { i1, i2 };
	bool updated = needUpdateUniformLoaction(location, floats, sizeof(floats));
	if (updated){
		glUniform2f(location, i1, i2);
	}
}

void GLProgram::setUniformLocationWith3f(GLint location, GLfloat i1, GLfloat i2, GLfloat i3)
{
	GLfloat floats[3] = { i1, i2 ,i3 };
	bool updated = needUpdateUniformLoaction(location, floats, sizeof(floats));
	if (updated){
		glUniform3f(location, i1, i2, i3);
	}
}

void GLProgram::setUniformLocationWith4f(GLint location, GLfloat i1, GLfloat i2, GLfloat i3, GLfloat i4)
{
	GLfloat floats[4] = { i1, i2, i3, i4 };
	bool updated = needUpdateUniformLoaction(location, floats, sizeof(floats));
	if (updated){
		glUniform4f(location, i1, i2, i3, i4);
	}
}

void GLProgram::setUniformLocationWith1fv(GLint location, GLfloat *floats, unsigned int numberOfArrays)
{
	bool updated = needUpdateUniformLoaction(location, floats, sizeof(float) * numberOfArrays);
	if (updated){
		glUniform1fv(location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith2fv(GLint location, GLfloat *floats, unsigned int numberOfArrays)
{
	bool updated = needUpdateUniformLoaction(location, floats, sizeof(float) * 2 * numberOfArrays);
	if (updated){
		glUniform2fv(location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith3fv(GLint location, GLfloat *floats, unsigned int numberOfArrays)
{
	bool updated = needUpdateUniformLoaction(location, floats, sizeof(float) * 3 * numberOfArrays);
	if (updated){
		glUniform3fv(location, (GLsizei)numberOfArrays, floats);
	}
}

void GLProgram::setUniformLocationWith4fv(GLint location, GLfloat *floats, unsigned int numberOfArrays)
{
	bool updated = needUpdateUniformLoaction(location, floats, sizeof(float) * 4 * numberOfArrays);
	if (updated){
		glUniform4fv(location, (GLsizei)numberOfArrays, floats);
	}
}


void GLProgram::setUniformLocationWithMatrix2fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices)
{
	bool updated = needUpdateUniformLoaction(location, matrixArray, sizeof(float) * 4 * numberOfMatrices);
	if (updated){
		glUniformMatrix2fv(location, (GLsizei)numberOfMatrices, GL_FALSE, matrixArray);
	}
}

void GLProgram::setUniformLocationWithMatrix3fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices)
{
	bool updated = needUpdateUniformLoaction(location, matrixArray, sizeof(float) * 9 * numberOfMatrices);
	if (updated){
		glUniformMatrix3fv(location, (GLsizei)numberOfMatrices, GL_FALSE, matrixArray);
	}
}

void GLProgram::setUniformLocationWithMatrix4fv(GLint location, const GLfloat* matrixArray, unsigned int numberOfMatrices)
{
	bool updated = needUpdateUniformLoaction(location, matrixArray, sizeof(float) * 16 * numberOfMatrices);
	if (updated){
		glUniformMatrix4fv(location, (GLsizei)numberOfMatrices, GL_FALSE, matrixArray);
	}
}

bool GLProgram::needUpdateUniformLoaction(GLint location, const GLvoid* data, unsigned int bytes)
{
	return true;

	if (location < 0){
		return false;
	}

	bool updated = true;

	auto element = m_hashForUniforms.find(location);
	if (element == m_hashForUniforms.end()){
		
		GLvoid* value = malloc(bytes);
		memcpy(value, data, bytes);
		m_hashForUniforms.insert(std::make_pair(location, value));
	}
	else
	{
		if (memcmp(element->second, data, bytes) == 0){
			updated = false;
		}
		else{
			memcpy(element->second, data, bytes);
		}
	}

	return updated;
}


void GLProgram::setUniformsForBuiltins()
{
	setUniformsForBuiltins(Engine::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW));
}

#include <iostream>
void GLProgram::setUniformsForBuiltins(const Mat4 &modelView)
{
	auto & matrixP = Engine::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	if (m_flags.useP){
		setUniformLocationWithMatrix4fv(m_uniformsLocation[UNIFORM_P_MATRIX], (GLfloat*)matrixP.data(), 1);
	}
	if (m_flags.useMV){
		setUniformLocationWithMatrix4fv(m_uniformsLocation[UNIFORM_MV_MATRIX], (GLfloat*)modelView.data(), 1);
	}
	if (m_flags.useMVP){
		Mat4 mvp = matrixP * modelView;
		setUniformLocationWithMatrix4fv(m_uniformsLocation[UNIFORM_MVP_MATRIX], (GLfloat*)mvp.data(), 1);
	}

	if (m_flags.useNormal){

		Mat3 normalMatrix = modelView.block(0,0,3,3);
		setUniformLocationWithMatrix3fv(m_uniformsLocation[UNIFORM_NORMAL_MATRIX], (GLfloat*)normalMatrix.data(), 1);
	}
}

void GLProgram::reset()
{
	m_vertShader = m_fragShader = 0;
	m_program = 0;

	memset(m_uniformsLocation, 0, sizeof(m_uniformsLocation));

	for (auto e : m_hashForUniforms){
		free(e.second);
	}

	m_hashForUniforms.clear();
}