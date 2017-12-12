#include "GL/glew.h"
#include "Renderer.h"
#include "../Comm/Utils.h"

#include "MeshCommand.h"
#include "GroupCommand.h"
#include "CustomCommand.h"


USING_NAMESPACE_G

// ±È½Ï
static bool compareRenderCommand(RenderCommand* a, RenderCommand* b)
{
	return a->getPriorityOrder() < b->getPriorityOrder();
}

void RenderQueue::pushBack(RenderCommand* command)
{
	m_queues.push_back(command);
}

unsigned int RenderQueue::size() const
{
	return m_queues.size();
}

void RenderQueue::sort()
{
	std::sort(std::begin(m_queues), std::end(m_queues), compareRenderCommand);
}

RenderCommand* RenderQueue::operator[](unsigned int index) const
{
	if (index < 0 || index > size()){
		return nullptr;
	}

	return m_queues[index];
}

void RenderQueue::clear()
{
	m_queues.clear();
}

void TransparentRenderQueue::pushBack(RenderCommand* command)
{
	m_queues.push_back(command);
}

unsigned int TransparentRenderQueue::size() const
{
	return m_queues.size();
}

RenderCommand* TransparentRenderQueue::operator[](unsigned int index) const
{
	if (index < 0 || index > size()){
		return nullptr;
	}

	return m_queues[index];
}

void TransparentRenderQueue::clear()
{
	m_queues.clear();
}

//  [11/28/2017 Administrator]

static const int DEFAULT_RENDER_QUEUE = 0;

Renderer::Renderer()
	: m_isRendering(false)
{
	// default initialize
	m_commandGroupStack.push(DEFAULT_RENDER_QUEUE);

	RenderQueue defaultRenderQueue;
	m_renderGroups.push_back(defaultRenderQueue);

	m_groupCommandMgr = new (std::nothrow) GroupCommandMgr();
}


Renderer::~Renderer()
{
	m_renderGroups.clear();

}

void Renderer::initGLView()
{

	//glGenBuffers(2, &m_bufferVBO[0]);


	m_glViewAssigned = true;
}

void Renderer::addCommand(RenderCommand* command)
{
	int renderQueue = m_commandGroupStack.top();
	this->addCommand(command, renderQueue);
}

void Renderer::addCommand(RenderCommand* command, int renderQueueId)
{
	if (m_isRendering || renderQueueId < 0 || command->getType() == RenderCommand::Type::UNKNOWN_COMMAND){
		G::log("warning : Renderer::addCommand  invalid RenderCommand !");
		return;
	}

	if (command->isTransparent()){
		m_transparentRenderGroups.pushBack(command);
	}
	else
	{
		m_renderGroups[renderQueueId].pushBack(command);
	}
}

void Renderer::pushGroup(int renderQueueID)
{
	if (m_isRendering){
		m_commandGroupStack.push(renderQueueID);
	}
}

void Renderer::popGroup()
{
	if (m_isRendering){
		m_commandGroupStack.pop();
	}
}

int Renderer::createRenderQueue()
{
	RenderQueue newRenderQueue;
	m_renderGroups.push_back(newRenderQueue);

	return (int)m_renderGroups.size() - 1;
}

void Renderer::clear()
{
	for (int i = 0, cnt = m_renderGroups.size(); i < cnt; ++i){
		m_renderGroups[i].clear();
	}

	m_transparentRenderGroups.clear();
}

void Renderer::render()
{
	m_isRendering = true;

	if (m_glViewAssigned){

		// sort
		for (auto &renderqueue : m_renderGroups){
			renderqueue.sort();
		}

		// opacity
		visitRenderQueue(m_renderGroups[0]);

		// transparent
		if (0 < m_transparentRenderGroups.size()){
			glDisable(GL_DEPTH_TEST);
			visitTransparentRenderQueue(m_transparentRenderGroups);
			glEnable(GL_DEPTH_TEST);
		}
	}

	clear();
	m_isRendering = false;
}

void Renderer::visitRenderQueue(const RenderQueue& queue)
{
	unsigned int size = queue.size();

	for (unsigned int index = 0; index < size; ++index)
	{
		auto command = queue[index];
		auto commandType = command->getType();

		if (RenderCommand::Type::MESH_COMMAND == commandType)
		{
			auto cmd = static_cast<MeshCommand*>(command);
			cmd->execute();
		}
		else if (RenderCommand::Type::GROUP_COMMAND == commandType)
		{
			auto cmd = static_cast<GroupCommand*>(command);
			auto renderQueueId = cmd->getRrenderQueueID();
			visitRenderQueue(m_renderGroups[renderQueueId]);
		}
		else if (RenderCommand::Type::CUSTOM_COMMAND == commandType)
		{
			auto cmd = static_cast<CustomCommand*>(command);
			cmd->execute();
		}
	}
}

void Renderer::visitTransparentRenderQueue(const TransparentRenderQueue& queue)
{
	unsigned int size = queue.size();

	for (unsigned int index = 0; index < size; ++index)
	{
		auto command = queue[index];
		auto cmdType = command->getType();

		if (RenderCommand::Type::MESH_COMMAND == cmdType)
		{
			auto cmd = static_cast<MeshCommand*>(command);
			cmd->execute();
		}
		else if (RenderCommand::Type::GROUP_COMMAND == cmdType)
 		{
			auto cmd = static_cast<GroupCommand*>(command);
			auto renderQueueId = cmd->getRrenderQueueID();
			visitRenderQueue(m_renderGroups[renderQueueId]);
		}
		else if (RenderCommand::Type::CUSTOM_COMMAND == cmdType)
		{
			auto cmd = static_cast<CustomCommand*>(command);
			cmd->execute();
		}
	}
}