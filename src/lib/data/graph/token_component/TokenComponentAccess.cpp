#include "data/graph/token_component/TokenComponentAccess.h"

std::string TokenComponentAccess::getAccessString(AccessType access)
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return "public";
	case ACCESS_PROTECTED:
		return "protected";
	case ACCESS_PRIVATE:
		return "private";
	case ACCESS_TEMPLATE:
		return "template parameter";
	case ACCESS_NONE:
		return "";
	}
	return "";
}

int TokenComponentAccess::typeToInt(AccessType type)
{
	return type;
}

TokenComponentAccess::AccessType TokenComponentAccess::intToType(int value)
{
	switch (value)
	{
	case 0x1:
		return ACCESS_PUBLIC;
	case 0x2:
		return ACCESS_PROTECTED;
	case 0x4:
		return ACCESS_PRIVATE;
	case 0x8:
		return ACCESS_TEMPLATE;
	default:
		break;
	}
	return ACCESS_NONE;
}

TokenComponentAccess::TokenComponentAccess(AccessType access)
	: m_access(access)
{
}

TokenComponentAccess::~TokenComponentAccess()
{
}

std::shared_ptr<TokenComponent> TokenComponentAccess::copy() const
{
	return std::make_shared<TokenComponentAccess>(*this);
}

TokenComponentAccess::AccessType TokenComponentAccess::getAccess() const
{
	return m_access;
}

std::string TokenComponentAccess::getAccessString() const
{
	return getAccessString(m_access);
}
