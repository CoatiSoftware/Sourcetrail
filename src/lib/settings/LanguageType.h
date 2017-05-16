#ifndef LANGUAGE_TYPE_H
#define LANGUAGE_TYPE_H

#include <string>

#include "settings/SourceGroupType.h"

enum LanguageType
{
	LANGUAGE_CPP,
	LANGUAGE_C,
	LANGUAGE_JAVA,
	LANGUAGE_UNKNOWN
};

std::string languageTypeToString(LanguageType t);
LanguageType stringToLanguageType(std::string s);

LanguageType getLanguageTypeForSourceGroupType(SourceGroupType t);

#endif // LANGUAGE_TYPE_H
