#ifndef NAME_DELIMITER_TYPE_H
#define NAME_DELIMITER_TYPE_H

#include <string>

enum NameDelimiterType
{
	NAME_DELIMITER_UNKNOWN,
	NAME_DELIMITER_FILE,
	NAME_DELIMITER_CXX,
	NAME_DELIMITER_JAVA
};

std::string nameDelimiterTypeToString(NameDelimiterType delimiter);
NameDelimiterType stringToNameDelimiterType(const std::string& s);

NameDelimiterType detectDelimiterType(const std::string& name);

#endif // NAME_DELIMITER_TYPE_H
