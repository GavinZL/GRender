/************************************************************************/
/*
* group render command
*/
/************************************************************************/

#ifndef  __GROUPCOMMAND_H__
#define  __GROUPCOMMAND_H__

#include "../Comm/Macors.h"
#include "RenderCommand.h"

#include <unordered_map>

NAMESPACE_BEGIN

class GroupCommandMgr
{
public:
	int getGroupID();
	void releaseGroupID(int groupID);

protected:
	friend class Renderer;

	GroupCommandMgr();
	~GroupCommandMgr();

	bool init();

protected:
	std::unordered_map<int, bool> m_groupMapping;

};


class GroupCommand : public RenderCommand
{
public:
	GroupCommand();
	~GroupCommand();

	void init();
	inline int getRrenderQueueID() const { return m_renderQueueID; }

protected:
	int m_renderQueueID;
};


NAMESPACE_END

#endif