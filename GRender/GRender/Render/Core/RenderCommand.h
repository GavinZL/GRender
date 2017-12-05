/************************************************************************/
/* ‰÷»æ√¸¡Óª˘¿‡ 
*
*/
/************************************************************************/

#ifndef  __RENDERCOMMAND_H__
#define  __RENDERCOMMAND_H__

#include "../Comm/Macors.h"
#include "../GDefine.h"

NAMESPACE_BEGIN


//! Blend Function used for textures
struct BlendFunc
{
	//! source blend function
	GLenum src;
	//! destination blend function
	GLenum dst;

	//! Blending disabled. Uses {GL_ONE, GL_ZERO}
	static const BlendFunc DISABLE;
	//! Blending enabled for textures with Alpha premultiplied. Uses {GL_ONE, GL_ONE_MINUS_SRC_ALPHA}
	static const BlendFunc ALPHA_PREMULTIPLIED;
	//! Blending enabled for textures with Alpha NON premultiplied. Uses {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA}
	static const BlendFunc ALPHA_NON_PREMULTIPLIED;
	//! Enables Additive blending. Uses {GL_SRC_ALPHA, GL_ONE}
	static const BlendFunc ADDITIVE;

	bool operator==(const BlendFunc &a) const
	{
		return src == a.src && dst == a.dst;
	}

	bool operator!=(const BlendFunc &a) const
	{
		return src != a.src || dst != a.dst;
	}

	bool operator<(const BlendFunc &a) const
	{
		return src < a.src || (src == a.src && dst < a.dst);
	}
};


/** Base class of the `RenderCommand` hierarchy.
*
The `Renderer` knows how to render `RenderCommands` objects.
*/
class RenderCommand
{
public:
	enum class Type
	{
		MESH_COMMAND,
		GROUP_COMMAND,
		CUSTOM_COMMAND,
		UNKNOWN_COMMAND
	};

	/** Returns the Command type */
	inline Type getType() const { return m_type; }

	/** Retruns whether is transparent */
	inline bool isTransparent() const { return m_isTransparent; }

	/** set transparent flag */
	inline void setTransparent(bool isTransparent) { m_isTransparent = isTransparent; }

protected:

	RenderCommand();
	virtual ~RenderCommand();


protected:

	/**transparent flag*/
	bool m_isTransparent;

	/** Type used in order to avoid dynamic cast, faster*/
	Type m_type;

};

NAMESPACE_END


#endif