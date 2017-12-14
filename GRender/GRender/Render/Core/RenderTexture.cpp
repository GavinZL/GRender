#include "GL/glew.h"
#include "RenderTexture.h"
#include "Engine.h"
#include "Renderer.h"
#include "../Comm/Utils.h"

USING_NAMESPACE_G

int ccNextPOT(int x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

RenderTexture::RenderTexture()
: m_FBO(0)
, m_oldFBO(0)
, m_depthRenderBuffer(0)
, m_texture(nullptr)
, m_pixelFormat(Texture2D::PixelFormat::RGBA8888)
, m_clearFlags(0)
, m_clearColor(Vec4::Zero())
, m_clearDepth(0.f)
, m_clearStencil(0)
, m_saveFileCallback(nullptr)
{

}

RenderTexture::~RenderTexture()
{
	glDeleteFramebuffers(1, &m_FBO);
	if (m_depthRenderBuffer){
		glDeleteFramebuffers(1, &m_depthRenderBuffer);
	}
}

RenderTexture* RenderTexture::create(int w, int h)
{
	RenderTexture* ret = new (std::nothrow) RenderTexture();
	if (ret && ret->initWithWidthAndHeight(w, h, Texture2D::PixelFormat::RGBA8888, 0)){
		return ret;
	}

	return nullptr;
}

RenderTexture* RenderTexture::create(int w, int h, Texture2D::PixelFormat format)
{
	RenderTexture* ret = new (std::nothrow) RenderTexture();
	if (ret && ret->initWithWidthAndHeight(w, h, format, 0)){
		return ret;
	}

	return nullptr;
}

RenderTexture* RenderTexture::create(int w, int h, Texture2D::PixelFormat format, GLuint depthStencilFormat)
{
	RenderTexture* ret = new (std::nothrow) RenderTexture();
	if (ret && ret->initWithWidthAndHeight(w, h, format, depthStencilFormat)){
		return ret;
	}

	return nullptr;
}

void RenderTexture::beginWithClearColor(const Vec4& color)
{
	beginWithClearColor(color, 0, 0, GL_COLOR_BUFFER_BIT);
}

void RenderTexture::beginWithClearColor(const Vec4& color, float depthValue)
{
	beginWithClearColor(color, depthValue, 0, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTexture::beginWithClearColor(const Vec4& color, float depthValue, int stencilValue)
{
	beginWithClearColor(color, depthValue, stencilValue, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderTexture::beginWithClearColor(const Vec4& color, float depthValue, int stencilValue, GLbitfield flags)
{
	this->setClearColor(color);
	this->setClearDepth(depthValue);
	this->setClearStencil(stencilValue);
	this->setClearFlag(flags);

	this->begin();

	// 清屏操作
	m_clearCommand.init(0);
	m_clearCommand.func = G_CALLBACK_0(RenderTexture::onClear, this);
	Engine::getInstance()->getRenderer()->addCommand(&m_clearCommand);
}

void RenderTexture::begin()
{
	//
	m_groupCommand.init(0);

	// add group command
	Renderer* renderer = Engine::getInstance()->getRenderer();
	renderer->addCommand(&m_groupCommand);
	renderer->pushGroup(m_groupCommand.getRrenderQueueID());

	// add begin command
	m_beginCommand.init(0);
	m_beginCommand.func = G_CALLBACK_0(RenderTexture::onBegin, this);
	renderer->addCommand(&m_beginCommand);
}

void RenderTexture::end()
{
	m_endCommand.init(0);
	m_endCommand.func = G_CALLBACK_0(RenderTexture::onEnd, this);

	Renderer* renderer = Engine::getInstance()->getRenderer();
	renderer->addCommand(&m_endCommand);
	renderer->popGroup();
}

void RenderTexture::clearColor(const Vec4& color)
{
	this->beginWithClearColor(color);
	this->end();
}

void RenderTexture::clearDepth(float depthValue)
{
	setClearDepth(depthValue);

	this->begin();
	m_clearDepthCommand.init(0);
	m_clearDepthCommand.func = G_CALLBACK_0(RenderTexture::onClearDepth, this);

	Engine::getInstance()->getRenderer()->addCommand(&m_clearDepthCommand);
	this->end();
}

void RenderTexture::clearStencil(int stencilValue)
{
	int stencilClearValue;
	glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencilClearValue);

	glClearStencil(stencilValue);
	glClear(GL_STENCIL_BUFFER_BIT);

	glClearStencil(stencilClearValue);
}

Image* RenderTexture::newImage(bool mirroImage)
{
	if (m_pixelFormat != Texture2D::PixelFormat::RGBA8888){
		G::log("Warning : only RGBA8888 can be saved as image !");
		return nullptr;
	}

	if (m_texture == nullptr){
		G::log("Warning : texture is empty !");
		return nullptr;
	}

	int w = m_texture->getWidth();
	int h = m_texture->getHeight();

	GLubyte* buffer = nullptr;
	GLubyte* tempData = nullptr;

	buffer = new (std::nothrow) GLubyte[w * h * 4];
	if (buffer == nullptr){
		return nullptr;
	}

	tempData = new (std::nothrow) GLubyte[w * h * 4];
	if (tempData == nullptr){
		delete[] buffer;
		buffer = nullptr;
		return nullptr;
	}

	Image* image = new (std::nothrow) Image();

	// bind buffer
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	// read pixels
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, tempData);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFBO);

	if (mirroImage){
		for (int i = 0; i < h; ++i){
			memcpy(&buffer[i * w * 4], &tempData[(h - i - 1) * w * 4], w * 4);
		}
		image->initWithRawData(buffer, w * h * 4, w, h, 8);
	}
	else{
		image->initWithRawData(tempData, w*h * 4, w, h, 8);
	}

	if (buffer){
		delete[] buffer;
		buffer = nullptr;
	}
	if (tempData){
		delete[] tempData;
		tempData = nullptr;
	}

	return image;
}

bool RenderTexture::saveToFile(const std::string& fileName, bool isRgba, std::function<void(RenderTexture*, const std::string&)> callback)
{
	std::string baseName(fileName.substr(fileName.length() - 5));
	std::transform(baseName.begin(), baseName.end(), baseName.begin(), ::tolower);

	if (baseName.find(".png") != std::string::npos){
		return saveToFile(fileName, Image::Format::PNG, isRgba, callback);
	}
	else if (baseName.find(".jpg") != std::string::npos || baseName.find(".jpeg") != std::string::npos){
		return saveToFile(fileName, Image::Format::JPG, false, callback);
	}
	else
	{
		G::log ("Warning: Only PNG and JPG format are supported now!");
	}

	return saveToFile(fileName, Image::Format::JPG, false, callback);
}

bool RenderTexture::saveToFile(const std::string& filename, Image::Format format, bool isRGBA, std::function<void(RenderTexture*, const std::string&)> callback)
{
	if (format != Image::Format::PNG && format != Image::Format::JPG){
		return false;
	}

	m_saveFileCallback = callback;

	m_saveToFileCommand.init(0);
	m_saveToFileCommand.func = G_CALLBACK_0(RenderTexture::onSaveToFile, this, filename, isRGBA);

	Engine::getInstance()->getRenderer()->addCommand(&m_saveToFileCommand);

	return true;
}


bool RenderTexture::initWithWidthAndHeight(int w, int h, Texture2D::PixelFormat format, GLuint depthStencilFormat)
{
	// get old frame buffer id
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFBO);

	int powW = w;
	int powH = h;

	// 默认支持不同分辨率的尺寸,如果不支持，还需要扩展为2的幂指数
	if (false){
		powW = ccNextPOT(w);
		powH = ccNextPOT(h);
	}

	// assign buffer
	auto dataLen = powH * powW * 4;
	void * data = malloc(dataLen);
	if (!data){
		return false;
	}
	memset(data, 0, dataLen);

	// create texture
	m_texture = new (std::nothrow)Texture2D();
	if (!m_texture){
		return false;
	}
	m_texture->initWithData(data, dataLen, m_pixelFormat, powW, powH);

	GLint oldRBO;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);

	// create and attach FBO
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getName(), 0);

	//create and attach depth buffer
	if (depthStencilFormat != 0){
		glGenRenderbuffers(1, &m_depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, depthStencilFormat, powW, powH); // allocate
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer); // binding

		if (depthStencilFormat == GL_DEPTH24_STENCIL8){
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);
		}
	}

	// check
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		return false;
	}

	GLenum drawBuffer[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFBO);

	return true;
}


//  [12/14/2017 Administrator]
void RenderTexture::onClear()
{
	// backup and set
	GLfloat oldClearColor[4] = { 0.0f };
	GLfloat oldDepthClearValue = 0.0f;
	GLint oldStencilClearValue = 0;

	// set clear color
	if (m_clearFlags & GL_COLOR_BUFFER_BIT)
	{
		glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);
		glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
	}

	if (m_clearFlags & GL_DEPTH_BUFFER_BIT)
	{
		glGetFloatv(GL_DEPTH_CLEAR_VALUE, &oldDepthClearValue);
		glClearDepth(m_clearDepth);
	}

	if (m_clearFlags & GL_STENCIL_BUFFER_BIT)
	{
		glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &oldStencilClearValue);
		glClearStencil(m_clearStencil);
	}

	// clear
	glClear(m_clearFlags);

	// restore prev color
	if (m_clearFlags & GL_COLOR_BUFFER_BIT)
	{
		glClearColor(oldClearColor[0], oldClearColor[1], oldClearColor[2], oldClearColor[3]);
	}
	if (m_clearFlags & GL_DEPTH_BUFFER_BIT)
	{
		glClearDepth(oldDepthClearValue);
	}
	if (m_clearFlags & GL_STENCIL_BUFFER_BIT)
	{
		glClearStencil(oldStencilClearValue);
	}
}

void RenderTexture::onClearDepth()
{
	GLfloat depthClearVlaue;
	glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthClearVlaue);

	glClearDepth(m_clearDepth);
	glClear(GL_DEPTH_BUFFER_BIT);

	glClearDepth(depthClearVlaue);
}

void RenderTexture::onBegin()
{
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void RenderTexture::onEnd()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFBO);
}

void RenderTexture::onSaveToFile(const std::string& filename, bool isRgb)
{
	Image *image = newImage();
	if (image){
		image->saveToFile(filename, isRgb);
	}
	if (m_saveFileCallback){
		m_saveFileCallback(this, filename);
	}

	delete image;
}