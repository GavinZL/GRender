

#include "RenderCommand.h"


USING_NAMESPACE_G

RenderCommand::RenderCommand()
: m_type(RenderCommand::Type::UNKNOWN_COMMAND)
, m_isTransparent(false)
{

}

RenderCommand::~RenderCommand()
{

}