#ifndef DEFINITION_TYPE_H
#define DEFINITION_TYPE_H

enum DefinitionType
{
	DEFINITION_NONE = 0,
	DEFINITION_IMPLICIT = 1,
	DEFINITION_EXPLICIT = 2
};

int definitionTypeToInt(DefinitionType definitionType);
DefinitionType intToDefinitionType(int definitionType);

#endif // DEFINITION_TYPE_H
