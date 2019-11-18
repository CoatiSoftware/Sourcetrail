#ifndef ELEMENT_COMPONENT_KIND_H
#define ELEMENT_COMPONENT_KIND_H

enum class ElementComponentKind
{
	NONE = 0,
	IS_AMBIGUOUS = 1
};

int elementComponentKindToInt(ElementComponentKind type);
ElementComponentKind intToElementComponentKind(int value);

#endif	  // ELEMENT_COMPONENT_KIND_H
