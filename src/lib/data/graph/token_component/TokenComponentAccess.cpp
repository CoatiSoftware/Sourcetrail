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
	case ACCESS_NONE:
		return "";
	}
	return "";
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
