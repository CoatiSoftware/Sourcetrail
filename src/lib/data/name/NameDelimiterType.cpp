#include "data/name/NameDelimiterType.h"

#include <vector>

std::string nameDelimiterTypeToString(NameDelimiterType delimiter)
{
	switch(delimiter)
	{
	case NAME_DELIMITER_FILE:
		return "/";
	case NAME_DELIMITER_CXX:
		return "::";
	case NAME_DELIMITER_JAVA:
		return ".";
	default:
		break;
	}
	return "@";
}

NameDelimiterType stringToNameDelimiterType(const std::string& s)
{
	if (s == nameDelimiterTypeToString(NAME_DELIMITER_FILE))
	{
		return NAME_DELIMITER_FILE;
	}
	if (s == nameDelimiterTypeToString(NAME_DELIMITER_CXX))
	{
		return NAME_DELIMITER_CXX;
	}
	if (s == nameDelimiterTypeToString(NAME_DELIMITER_JAVA))
	{
		return NAME_DELIMITER_JAVA;
	}
	return NAME_DELIMITER_UNKNOWN;
}

NameDelimiterType detectDelimiterType(const std::string& name)
{
	std::vector<NameDelimiterType> allDelimiters {NAME_DELIMITER_FILE, NAME_DELIMITER_CXX, NAME_DELIMITER_JAVA};

	for (NameDelimiterType delimiter: allDelimiters)
	{
		if (name.find(nameDelimiterTypeToString(delimiter)) != std::string::npos)
		{
			return delimiter;
		}
	}

	return NAME_DELIMITER_UNKNOWN;
}
