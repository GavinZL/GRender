/************************************************************************/
/* ‰÷»æmesh command 
*
*/
/************************************************************************/

#ifndef  __MESHCOMMAND_H__
#define  __MESHCOMMAND_H__

#include "../Comm/Macors.h"

#include "RenderCommand.h"
#include "GLProgram.h"

NAMESPACE_BEGIN

class Mesh;
class MeshCommand : public RenderCommand
{
public:
	MeshCommand();
	~MeshCommand();

	/**Initialize with texture */
	void init(GLuint textureId, GLProgramState* glProgramState, BlendFunc blendType,
		GLuint vertexBuffer, GLuint indexBuffer, GLenum primitive, GLenum indexType,
		long long indexCount, const Mat4& mv);

	/**Initialize with color*/
	void init(GLProgramState* glProgramState, GLuint vertexBuffer, GLuint vertexColor,
		GLuint indexBuffer, GLenum primitive, GLenum indexType, long long indexCount, const Mat4& mv);

	/**Initialize with mesh*/
	void init(Mesh* mesh, const Mat4& mv);


	/**GL states*/
	void setCullFaceEnabled(bool en);
	void setCullface(GLenum cuffFace);

	void setDepthTestEnabled(bool en);
	void setDepthWriteEnabled(bool en);

	void setDisplayColor(const Vec4& color);

	void setLightMask(unsigned int lightMask);

	/**execute*/
	void execute();

protected:

	/**apply render states*/
	void applyRenderState();

	/**restore all states*/
	void restoreRenderState();

	/**set lights uniforms*/
	void setLightUniforms();
	void resetLightUniforms();

protected:
	/**is texture  or color render */
	bool m_isTextureRenderer;

	/** texture id*/
	GLuint m_textureId;

	/** glProgramState */
	GLProgramState* m_glProgramState;

	/**vertex position buffer id*/
	GLuint m_vertexBuffer;

	/**vertex color buffer id*/
	GLuint m_colorBuffer;

	/**normal buffer id*/
	GLuint m_normalBuffer;
	
	/**vertex index buffer id*/
	GLuint m_indexBuffer;

	/**draw type*/
	GLenum m_primitive;

	/** GL_BYTE or GL_FLOAT ...*/
	GLenum m_indexFormat;

	/** index count */
	long long m_indexCount;

	/**draw attributes*/
	bool m_callFaceEnabled;
	bool m_depthTestEnabled;
	bool m_depthWriteEnabled;
	GLenum m_cullFace;

	/**model view matrix*/
	Mat4 m_mv;

	/**light mask*/
	unsigned int m_lightMask;

	/**mesh */
	Mesh* m_mesh;
};

NAMESPACE_END

#endif