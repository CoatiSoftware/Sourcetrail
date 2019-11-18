#include "AccessKind.h"

AccessKind intToAccessKind(int v)
{
	switch (v)
	{
	case ACCESS_PUBLIC:
		return ACCESS_PUBLIC;
	case ACCESS_PROTECTED:
		return ACCESS_PROTECTED;
	case ACCESS_PRIVATE:
		return ACCESS_PRIVATE;
	case ACCESS_DEFAULT:
		return ACCESS_DEFAULT;
	case ACCESS_TEMPLATE_PARAMETER:
		return ACCESS_TEMPLATE_PARAMETER;
	case ACCESS_TYPE_PARAMETER:
		return ACCESS_TYPE_PARAMETER;
	}
	return ACCESS_NONE;
}

int accessKindToInt(AccessKind t)
{
	return t;
}

std::wstring accessKindToString(AccessKind t)
{
	switch (t)
	{
	case ACCESS_NONE:
		return L"";
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
