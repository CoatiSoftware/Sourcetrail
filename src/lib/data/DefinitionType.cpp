#include "data/DefinitionType.h"

int definitionTypeToInt(DefinitionType definitionType)
{
	return definitionType;
}

DefinitionType intToDefinitionType(int definitionType)
{
	switch (definitionType)
	{
	case 0:
		return DEFINITION_NONE;
	case 1:
		return DEFINITION_IMPLICIT;
	case 2:
		return DEFINITION_EXPLICIT;
	}
	return DEFINITION_NONE;
}
