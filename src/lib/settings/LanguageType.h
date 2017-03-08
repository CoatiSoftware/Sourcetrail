#ifndef LANGUAGE_TYPE_H
#define LANGUAGE_TYPE_H

#include <string>

#include "settings/ProjectType.h"

enum LanguageType
{
	LANGUAGE_C,
	LANGUAGE_CPP,
	LANGUAGE_JAVA,
	LANGUAGE_UNKNOWN
};

std::string languageTypeToString(LanguageType t);
LanguageType stringToLanguageType(std::string s);

std::string getSymbolNameDelimiterForLanguage(LanguageType t);
LanguageType getLanguageTypeForProjectType(ProjectType t);

#endif // LANGUAGE_TYPE_H
