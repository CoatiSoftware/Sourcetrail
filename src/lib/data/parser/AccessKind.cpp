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

