#include "GroupType.h"

std::wstring groupTypeToString(GroupType type)
{
	switch (type)
	{
	case GroupType::NONE:
		return L"none";
	case GroupType::DEFAULT:
		return L"default";
	case GroupType::FRAMELESS:
		return L"frameless";
	case GroupType::FILE:
		return L"file";
	case GroupType::NAMESPACE:
		return L"namespace";
	case GroupType::INHERITANCE:
		return L"inheritance";
	}

	return L"none";
}

GroupType stringToGroupType(const std::wstring& value)
{
	if (value == groupTypeToString(GroupType::NONE))
		return GroupType::NONE;
	if (value == groupTypeToString(GroupType::DEFAULT))
		return GroupType::DEFAULT;
	if (value == groupTypeToString(GroupType::FRAMELESS))
		return GroupType::FRAMELESS;
	if (value == groupTypeToString(GroupType::FILE))
		return GroupType::FILE;
	if (value == groupTypeToString(GroupType::NAMESPACE))
		return GroupType::NAMESPACE;
	if (value == groupTypeToString(GroupType::INHERITANCE))
		return GroupType::INHERITANCE;

	return GroupType::NONE;
}
