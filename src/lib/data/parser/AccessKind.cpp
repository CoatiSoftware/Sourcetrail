#include "data/parser/AccessKind.h"

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

std::string accessKindToString(AccessKind t)
{
	switch (t)
	{
	case ACCESS_NONE:
		return "";
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

