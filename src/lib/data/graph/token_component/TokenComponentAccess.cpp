#include "TokenComponentAccess.h"

std::wstring TokenComponentAccess::getAccessString(AccessKind access)
{
	switch (access)
	{
	case ACCESS_NONE:
		break;
	case ACCESS_PUBLIC:
		return L"public";
	case ACCESS_PROTECTED:
		return L"protected";
	case ACCESS_PRIVATE:
		return L"private";
	case ACCESS_DEFAULT:
		return L"default";
	case ACCESS_TEMPLATE_PARAMETER:
		return L"template parameter";
	case ACCESS_TYPE_PARAMETER:
		return L"type parameter";
	}
	return L"";
}


TokenComponentAccess::TokenComponentAccess(AccessKind access): m_access(access) {}

TokenComponentAccess::~TokenComponentAccess() {}

std::shared_ptr<TokenComponent> TokenComponentAccess::copy() const
{
	return std::make_shared<TokenComponentAccess>(*this);
}

AccessKind TokenComponentAccess::getAccess() const
{
	return m_access;
}

std::wstring TokenComponentAccess::getAccessString() const
{
	return getAccessString(m_access);
}
