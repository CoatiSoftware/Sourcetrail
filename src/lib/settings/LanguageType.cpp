#include "settings/LanguageType.h"

std::string languageTypeToString(LanguageType t)
{
	switch (t)
	{
	case LANGUAGE_C:
		return "C";
	case LANGUAGE_CPP:
		return "C++";
	case LANGUAGE_JAVA:
		return "Java";
	case LANGUAGE_UNKNOWN:
		break;
	}
	return "unknown";
}

LanguageType stringToLanguageType(std::string s)
{
	if (s == "C")
	{
		return LANGUAGE_C;
	}
	else if (s == "C++")
	{
		return LANGUAGE_CPP;
	}
	else if (s == "Java")
	{
		return LANGUAGE_JAVA;
	}
	return LANGUAGE_UNKNOWN;
}

std::string getSymbolNameDelimiterForLanguage(LanguageType t)
{
	switch (t)
	{
	case LANGUAGE_C:
	case LANGUAGE_CPP:
		return "::";
	case LANGUAGE_JAVA:
		return ".";
	case LANGUAGE_UNKNOWN:
		break;
	}
	return "@";
}

LanguageType getLanguageTypeForProjectType(ProjectType t)
{
	switch (t)
	{
	case PROJECT_C_EMPTY:
		return LANGUAGE_C;
	case PROJECT_CPP_EMPTY:
		return LANGUAGE_CPP;
	case PROJECT_CXX_CDB:
		return LANGUAGE_CPP;
	case PROJECT_CXX_VS:
		return LANGUAGE_CPP;
	case PROJECT_JAVA_EMPTY:
		return LANGUAGE_JAVA;
	case PROJECT_JAVA_MAVEN:
		return LANGUAGE_JAVA;
	default:
		break;
	}

	return LANGUAGE_UNKNOWN;
}

