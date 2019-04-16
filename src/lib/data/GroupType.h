#ifndef GROUP_TYPE_H
#define GROUP_TYPE_H

#include <string>

enum class GroupType
{
	NONE,
	DEFAULT,
	FRAMELESS,
	FILE,
	NAMESPACE,
	INHERITANCE
};

std::wstring groupTypeToString(GroupType type);
GroupType stringToGroupType(const std::wstring& value);

enum class GroupLayout
{
	LIST,
	SKEWED,
	BUCKET,
	SQUARE
};

#endif // GROUP_TYPE_H
