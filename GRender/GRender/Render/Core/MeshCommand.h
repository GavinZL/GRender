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

	/**Initialize with mesh*/
	void init(Mesh* mesh, const Mat4& mv, unsigned int priority = 0);


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
	/**mesh */
	Mesh* m_mesh;

	/**is texture  or color render */
	bool m_isTextureRenderer;

	/**draw type*/
	GLenum m_primitive;

	/**draw attributes*/
	bool m_callFaceEnabled;
	bool m_depthTestEnabled;
	bool m_depthWriteEnabled;
	GLenum m_cullFace;

	/**model view matrix*/
	Mat4 m_mv;

	/**light mask*/
	unsigned int m_lightMask;


};

NAMESPACE_END

#endif