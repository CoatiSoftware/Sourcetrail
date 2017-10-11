#include "data/DefinitionKind.h"

int definitionKindToInt(DefinitionKind definitionKind)
{
	return definitionKind;
}

DefinitionKind intToDefinitionKind(int definitionKind)
{
	switch (definitionKind)
	{
	case DEFINITION_NONE:
		return DEFINITION_NONE;
	case DEFINITION_IMPLICIT:
		return DEFINITION_IMPLICIT;
	case DEFINITION_EXPLICIT:
		return DEFINITION_EXPLICIT;
	}
	return DEFINITION_NONE;
}
