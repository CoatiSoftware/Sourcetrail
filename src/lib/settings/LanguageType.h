#ifndef LANGUAGE_TYPE_H
#define LANGUAGE_TYPE_H

#include <string>

#include "SourceGroupType.h"

enum LanguageType
{
#if BUILD_CXX_LANGUAGE_PACKAGE
	LANGUAGE_CPP,
	LANGUAGE_C,
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	LANGUAGE_JAVA,
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
	LANGUAGE_PYTHON,
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE
	LANGUAGE_CUSTOM,
	LANGUAGE_UNKNOWN
};

std::string languageTypeToString(LanguageType t);
LanguageType stringToLanguageType(std::string s);

LanguageType getLanguageTypeForSourceGroupType(SourceGroupType t);

#endif	  // LANGUAGE_TYPE_H
