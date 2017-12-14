/************************************************************************/
/* 
* render to  texture
*/
/************************************************************************/

#ifndef  _RENDERTEXTURE_H__
#define  _RENDERTEXTURE_H__

#include "../Comm/Macors.h"
#include "../GDefine.h"
#include "Node.h"
#include "Texture2D.h"
#include "Image.h"
#include "GroupCommand.h"
#include "CustomCommand.h"

#include <functional>

NAMESPACE_BEGIN


/**
@brief RenderTexture is a generic rendering target. To render things into it,
simply construct a render target, call begin on it, call visit on any cocos
scenes or objects to render them, and call end. 
*/
class RenderTexture : public Node
{
public:
	/**
	* static function for create
	*
	* params:
	*	w : width
	*	h : height
	*	format: a pixel format, only RGB and RGBA formats are valid [RGBA8888 or RGB888]
	*	depthStencilFormat: depthStencil format
	*/
	static RenderTexture* create(int w, int h);
	static RenderTexture* create(int w, int h, Texture2D::PixelFormat format);
	static RenderTexture* create(int w, int h, Texture2D::PixelFormat format, GLuint depthStencilFormat);

	/**
	* start grabbing frame buffer
	*
	* params:
	*	color : glClearColor's color
	*	depthValue : glClearDepth 's value
	*	stencilValue : glClearStencil 's value
	*/
	virtual void beginWithClearColor(const Vec4& color);
	virtual void beginWithClearColor(const Vec4& color, float depthValue);
	virtual void beginWithClearColor(const Vec4& color, float depthValue, int stencilValue);
	virtual void beginWithClearColor(const Vec4& color, float depthValue, int stencilValue, GLbitfield flags);
	
	/**begin grabbing*/
	virtual void begin();

	/**ends grabbing frame buffer*/
	virtual void end();

	/**
	* clear color & depth & stencil
	*/
	virtual void clearColor(const Vec4& color);
	virtual void clearDepth(float depthValue);
	virtual void clearStencil(int stencilValue);

	/**
	* set && get color & depthh & stencil
	*/
	inline const Vec4& getClearColor() const { return m_clearColor; }
	inline void setClearColor(const Vec4& color){ m_clearColor = color; }

	inline float getClearDepth() const{ return m_clearDepth; }
	inline void setClearDepth(float depth) { m_clearDepth = depth; }

	inline int getClearStencil() const { return m_clearStencil; }
	inline void setClearStencil(int stencil){ m_clearStencil = stencil; }


	/**
	*  image && file 
	*	mirroImage : 是否需要镜像
	*/
	Image* newImage(bool mirroImage = true);
	bool saveToFile(const std::string& fileName, bool isRgba = true, std::function<void(RenderTexture*, const std::string&)> callback = nullptr);
	bool saveToFile(const std::string& filename, Image::Format format, bool isRGBA = true, std::function<void(RenderTexture*, const std::string&)> callback = nullptr);


	/**
	*clear flags : GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT
	*
	*/
	inline unsigned int getClearFlag() const { return m_clearFlags; }
	inline void setClearFlag(unsigned int flags){ m_clearFlags = flags; }


public:
	RenderTexture();
	virtual ~RenderTexture();

	/** initializes a RenderTexture object with width and height in Points and a pixel format( only RGB and RGBA formats are valid ) and depthStencil format*/
	bool initWithWidthAndHeight(int w, int h, Texture2D::PixelFormat format, GLuint depthStencilFormat);


protected:
	// 处理回调事件
	void onClear();
	void onClearDepth();

	void onBegin();
	void onEnd();

	void onSaveToFile(const std::string& filename, bool isRgb = true);

protected:

	/**frame buffer object*/
	GLuint m_FBO;
	GLint m_oldFBO;

	/**render buffer */
	GLuint m_depthRenderBuffer;

	/**render texture*/
	Texture2D* m_texture;
	Texture2D::PixelFormat m_pixelFormat;

	/**clear flags*/
	GLbitfield m_clearFlags;

	/**clear value*/
	G::Color4 m_clearColor;
	GLclampf m_clearDepth;
	GLint m_clearStencil;

	/**render commands*/
	// 创建一个新的渲染队列
	GroupCommand m_groupCommand;
	CustomCommand m_beginCommand;
	CustomCommand m_endCommand;
	CustomCommand m_clearCommand;
	CustomCommand m_clearDepthCommand;

	/**save render texture to file*/
	CustomCommand m_saveToFileCommand;
	std::function<void(RenderTexture*, const std::string&)> m_saveFileCallback;
};


NAMESPACE_END

#endif