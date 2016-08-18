#ifndef LANGUAGE_TYPE_H
#define LANGUAGE_TYPE_H

#include <string>

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

#endif // LANGUAGE_TYPE_H
