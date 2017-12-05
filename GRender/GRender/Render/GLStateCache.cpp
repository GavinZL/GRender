/************************************************************************/
/* 
* 头文件包含file
*/
/************************************************************************/
#include "GL/glew.h"
#include <cstdint>
#include "Comm/Macors.h"
#include "Comm/Utils.h"

#include "GLStateCache.h"

#ifndef CC_ENABLE_GL_STATE_CACHE
#define CC_ENABLE_GL_STATE_CACHE 1
#endif

NAMESPACE_BEGIN

static const int MAX_ATTRIBUTES = 16;
static const int MAX_ACTIVE_TEXTURE = 16;

namespace
{
	static GLuint s_currentProjectionMatrix = -1;
	static uint32_t s_attributeFlags = 0;  // 32 attributes max

#if CC_ENABLE_GL_STATE_CACHE

	static GLuint    s_currentShaderProgram = -1;
	static GLuint    s_currentBoundTexture[MAX_ACTIVE_TEXTURE] = { (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, (GLuint)-1, };
	static GLenum    s_blendingSource = -1;
	static GLenum    s_blendingDest = -1;
	static int       s_GLServerState = 0;
	static GLuint    s_VAO = 0;
	static GLenum    s_activeTexture = -1;

#endif // CC_ENABLE_GL_STATE_CACHE
}


void bindTexture2D(GLuint textureId)
{
	bindTexture2DN(0, textureId);
}


void bindTexture2DN(GLuint textureUnit, GLuint textureId)
{
#if CC_ENABLE_GL_STATE_CACHE
	if (textureUnit >= MAX_ACTIVE_TEXTURE){
		G::log("textureUnit is too big");
		return;
	}

	if (s_currentBoundTexture[textureUnit] != textureId)
	{
		s_currentBoundTexture[textureUnit] = textureId;
		activeTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
#else
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureId);
#endif
}

void activeTexture(GLenum texture)
{
#if CC_ENABLE_GL_STATE_CACHE
	if (s_activeTexture != texture) {
		s_activeTexture = texture;
		glActiveTexture(s_activeTexture);
	}
#else
	glActiveTexture(texture);
#endif
}

void deleteTexture(GLuint textureId)
{
#if CC_ENABLE_GL_STATE_CACHE
	for (size_t i = 0; i < MAX_ACTIVE_TEXTURE; ++i)
	{
		if (s_currentBoundTexture[i] == textureId)
		{
			s_currentBoundTexture[i] = -1;
		}
	}
#endif // CC_ENABLE_GL_STATE_CACHE

	glDeleteTextures(1, &textureId);
}



void enableVertexAttributes(uint32_t flags)
{
	// bindVao(0)

	for (int i = 0; i < MAX_ATTRIBUTES; ++i){
		unsigned int bit = 1 << i;
		
		bool enabled = flags & bit;
		bool enabledBefore = s_attributeFlags & bit;

		if (enabled != enabledBefore){
			if (enabled){
				glEnableVertexAttribArray(i);
			}
			else
			{
				glDisableVertexAttribArray(i);
			}
		}
	}

	s_attributeFlags = flags;
}

NAMESPACE_END
