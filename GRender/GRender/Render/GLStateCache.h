/************************************************************************/
/* 
* 头文件包含file
*/
/************************************************************************/


#ifndef  __GLSTATECACHE_H__
#define  __GLSTATECACHE_H__

#include <cstdint>
#include "Comm/Macors.h"
#include "Core/GLProgram.h"

NAMESPACE_BEGIN

/** vertex attrib flags */
enum {
	VERTEX_ATTRIB_FLAG_NONE = 0,

	VERTEX_ATTRIB_FLAG_POSITION = 1 << 0,
	VERTEX_ATTRIB_FLAG_COLOR = 1 << 1,
	VERTEX_ATTRIB_FLAG_TEX_COORD = 1 << 2,
	VERTEX_ATTRIB_FLAG_NORMAL = 1 << 3,

	VERTEX_ATTRIB_FLAG_POS_COLOR_TEX_NORMAL = 
	(VERTEX_ATTRIB_FLAG_POSITION | VERTEX_ATTRIB_FLAG_COLOR | VERTEX_ATTRIB_FLAG_TEX_COORD | VERTEX_ATTRIB_FLAG_NORMAL),
};

/** If the texture is not already bound to texture unit 0, it binds it.
If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glBindTexture() directly.
@since v2.0.0
*/
void bindTexture2D(GLuint textureId);


/** If the texture is not already bound to a given unit, it binds it.
If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glBindTexture() directly.
@since v2.1.0
*/
void bindTexture2DN(GLuint textureUnit, GLuint textureId);


/** It will delete a given texture. If the texture was bound, it will invalidate the cached.
If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glDeleteTextures() directly.
@since v2.0.0
*/
void deleteTexture(GLuint textureId);

/** Select active texture unit.
If CC_ENABLE_GL_STATE_CACHE is disabled, it will call glActiveTexture() directly.
@since v3.0
*/
void activeTexture(GLenum texture);


/** Will enable the vertex attributes that are passed as flags.
Possible flags:

* VERTEX_ATTRIB_FLAG_POSITION
* VERTEX_ATTRIB_FLAG_COLOR
* VERTEX_ATTRIB_FLAG_TEX_COORDS

These flags can be ORed. The flags that are not present, will be disabled.

@since v2.0.0
*/
void enableVertexAttributes(uint32_t flags);

NAMESPACE_END


#endif