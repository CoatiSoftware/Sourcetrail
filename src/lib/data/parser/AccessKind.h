#ifndef ACCESS_KIND_H
#define ACCESS_KIND_H

enum AccessKind
{ // these values need to be the same as AccessKind in Java code
	ACCESS_NONE = 0,
	ACCESS_PUBLIC = 1,
	ACCESS_PROTECTED = 2,
	ACCESS_PRIVATE = 3,
	ACCESS_DEFAULT = 4,
	ACCESS_TEMPLATE_PARAMETER = 5,
	ACCESS_TYPE_PARAMETER = 6
};

AccessKind intToAccessKind(int v);
int accessKindToInt(AccessKind t);

#endif // ACCESS_KIND_H
