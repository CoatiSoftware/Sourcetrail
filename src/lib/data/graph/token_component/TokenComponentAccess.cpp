#include "data/graph/token_component/TokenComponentAccess.h"

std::string TokenComponentAccess::getAccessString(AccessKind access)
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return "public";
	case ACCESS_PROTECTED:
		return "protected";
	case ACCESS_PRIVATE:
		return "private";
	case ACCESS_DEFAULT:
		return "default";
	case ACCESS_TEMPLATE_PARAMETER:
		return "template parameter";
	case ACCESS_TYPE_PARAMETER:
		return "type parameter";
	}
	return "";
}


TokenComponentAccess::TokenComponentAccess(AccessKind access)
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

AccessKind TokenComponentAccess::getAccess() const
{
	return m_access;
}

std::string TokenComponentAccess::getAccessString() const
{
	return getAccessString(m_access);
}
