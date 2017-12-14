/************************************************************************/
/* 
* custom render command
*/
/************************************************************************/

#ifndef  __CUSTOMCOMMAND_H__
#define  __CUSTOMCOMMAND_H__

#include "../Comm/Macors.h"
#include "RenderCommand.h"

#include <functional>

NAMESPACE_BEGIN


class CustomCommand : public RenderCommand
{
public:
	CustomCommand();
	~CustomCommand();

public:

	void init(int priority=0);
	void execute();

	inline bool isTranslucent() { return false; }

	std::function<void()> func;
};


NAMESPACE_END

#endif