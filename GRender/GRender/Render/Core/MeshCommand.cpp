#include "MeshCommand.h"


USING_NAMESPACE_G


MeshCommand::MeshCommand()
{

}

MeshCommand::~MeshCommand()
{

}

/**Initialize*/
void MeshCommand::init(GLuint textureId, GLProgramState* glProgramState, BlendFunc blendType,
	GLuint vertexBuffer, GLuint indexBuffer, GLenum primitive, GLenum indexType,
	long long indexCount, const Mat4& mv)
{

}


/**GL states*/
void MeshCommand::setCullFaceEnabled(bool en)
{

}

void MeshCommand::setCullface(GLenum cuffFace)
{

}


void MeshCommand::setDepthTestEnabled(bool en)
{

}

void MeshCommand::setDepthWriteEnabled(bool en)
{

}

void MeshCommand::setDisplayColor(const Vec4& color)
{

}

void MeshCommand::setLightMask(unsigned int lightMask)
{

}

/**execute*/
void MeshCommand::execute()
{

}

/**apply render states*/
void MeshCommand::applyRenderState()
{

}

/**restore all states*/
void MeshCommand::restoreRenderState()
{

}

/**set lights uniforms*/
void MeshCommand::setLightUniforms()
{

}

void MeshCommand::resetLightUniforms()
{

}