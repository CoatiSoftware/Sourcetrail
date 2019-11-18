#include "ElementComponentKind.h"

int elementComponentKindToInt(ElementComponentKind kind)
{
	return static_cast<int>(kind);
}

ElementComponentKind intToElementComponentKind(int value)
{
	const ElementComponentKind kinds[] = {ElementComponentKind::IS_AMBIGUOUS};

	for (ElementComponentKind kind: kinds)
	{
		if (value == elementComponentKindToInt(kind))
		{
			return kind;
		}
	}

	return ElementComponentKind::NONE;
}
