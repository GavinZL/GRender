#include "GL/glew.h"
#include "CustomCommand.h"

USING_NAMESPACE_G


CustomCommand::CustomCommand()
: func(nullptr)
{
	m_type = RenderCommand::Type::CUSTOM_COMMAND;
}

CustomCommand::~CustomCommand()
{

}

void CustomCommand::init(int priority)
{
	m_priorityOrder = priority;
}

void CustomCommand::execute()
{
	if (func)
	{
		func();
	}
}
