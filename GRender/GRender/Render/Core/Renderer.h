/************************************************************************/
/* 整个引擎渲染类 
*
*/
/************************************************************************/


#ifndef  __RENDERER_H__
#define  __RENDERER_H__

#include "../Comm/Macors.h"
#include "RenderCommand.h"

#include <vector>
#include <stack>

NAMESPACE_BEGIN

class MeshCommand;

/** Class that knows how to sort `RenderCommand` objects.
Since the commands that have `z == 0` are "pushed back" in
the correct order, the only `RenderCommand` objects that need to be sorted,
are the ones that have `z < 0` and `z > 0`.
*/
class RenderQueue
{
public:
	void pushBack(RenderCommand* command);
	unsigned int size() const;
	void sort();
	RenderCommand* operator[](unsigned int idnex) const;
	void clear();

protected:

	std::vector<RenderCommand*> m_queues;
};

class TransparentRenderQueue
{
public:
	void pushBack(RenderCommand* command);
	unsigned int size() const;

	RenderCommand* operator[](unsigned int  index) const;
	void clear();

protected:
	std::vector<RenderCommand*> m_queues;
};


struct RenderStackElement
{
	int renderQueueID;
	unsigned int currentIndex;
};

class GroupCommandMgr;

class Renderer
{
public:

	Renderer();
	~Renderer();

	/**TODO: manage GLView inside Render itself*/
	void initGLView();

	/** Adds a `RenderComamnd` into the renderer */
	void addCommand(RenderCommand* command);
	void addCommand(RenderCommand* command, int renderQueueId);

	/** Pushes a group into the render queue */
	void pushGroup(int renderQueueID);
	void popGroup();

	/** Creates a render queue and returns its Id */
	int createRenderQueue();

	/** Cleans all `RenderCommand`s in the queue */
	void clear();

	/** Renders into the GLView all the queued `RenderCommand` objects */
	void render();


	inline GroupCommandMgr* getGroupCommandMgr() const { return m_groupCommandMgr; }

protected:

	/**visit renderQueue ' s renderCommand and execute */
	void visitRenderQueue(const RenderQueue& queue);
	void visitTransparentRenderQueue(const TransparentRenderQueue& queue);

protected:

	/**stack for current render queue*/
	std::stack<int> m_commandGroupStack;

	/**Render Groups*/
	std::vector<RenderQueue> m_renderGroups;

	/**one transparent render queue*/
	TransparentRenderQueue m_transparentRenderGroups;

	/**the flag for checking whether renderer is gl initialized*/
	bool m_glViewAssigned;

	/**the flag for checking whether renderer is rendering*/
	bool m_isRendering;

	/**vertex buffer -> 0 : vertex, 1 : indeces*/
	GLuint m_bufferVBO[2];	


	GroupCommandMgr *m_groupCommandMgr;

};

NAMESPACE_END

#endif