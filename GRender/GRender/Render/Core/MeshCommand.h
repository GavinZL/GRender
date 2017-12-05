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

class MeshCommand : public RenderCommand
{
public:
	MeshCommand();
	~MeshCommand();

	/**Initialize*/
	void init(GLuint textureId, GLProgramState* glProgramState, BlendFunc blendType,
		GLuint vertexBuffer, GLuint indexBuffer, GLenum primitive, GLenum indexType,
		long long indexCount, const Mat4& mv);


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

	/** texture id*/
	GLuint m_textureId;

	/** glProgramState */
	GLProgramState* m_glProgramState;

	GLuint m_vertexBuffer;
	GLuint m_indexBuffer;
	GLenum m_primitive;
	GLenum m_indexFormat;
	long long m_indexCount;


	bool m_callFaceEnabled;
	bool m_depthTestEnabled;
	bool m_depthWriteEnabled;
	GLenum m_cullFace;

	Mat4 m_mv;
};

NAMESPACE_END

#endif