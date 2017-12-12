#include "GL/glew.h"
#include "GroupCommand.h"
#include "Engine.h"
#include "Renderer.h"

USING_NAMESPACE_G


GroupCommandMgr::GroupCommandMgr()
{

}

GroupCommandMgr::~GroupCommandMgr()
{

}

bool GroupCommandMgr::init()
{
	m_groupMapping[0] = true;
	return true;
}


int GroupCommandMgr::getGroupID()
{
	for (auto it = m_groupMapping.begin(); it != m_groupMapping.end(); ++it)
	{
		if (!it->second)
		{
			m_groupMapping[it->first] = true;
			return it->first;
		}
	}

	int newId = Engine::getInstance()->getRenderer()->createRenderQueue();
	m_groupMapping[newId] = true;

	return newId;
}

void GroupCommandMgr::releaseGroupID(int groupID)
{
	m_groupMapping[groupID] = false;
}

// [11/28/2017 Administrator]

GroupCommand::GroupCommand()
{
	m_type = RenderCommand::Type::GROUP_COMMAND;
	m_renderQueueID = Engine::getInstance()->getRenderer()->getGroupCommandMgr()->getGroupID();
}

void GroupCommand::init(int priority)
{
	m_priorityOrder = priority;
	auto manager = Engine::getInstance()->getRenderer()->getGroupCommandMgr();
	manager->releaseGroupID(m_renderQueueID);
	m_renderQueueID = manager->getGroupID();
}

GroupCommand::~GroupCommand()
{
	Engine::getInstance()->getRenderer()->getGroupCommandMgr()->releaseGroupID(m_renderQueueID);
}