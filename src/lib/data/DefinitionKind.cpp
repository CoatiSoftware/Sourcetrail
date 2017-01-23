#include "data/DefinitionKind.h"

int definitionKindToInt(DefinitionKind definitionKind)
{
	return definitionKind;
}

DefinitionKind intToDefinitionKind(int definitionKind)
{
	switch (definitionKind)
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
