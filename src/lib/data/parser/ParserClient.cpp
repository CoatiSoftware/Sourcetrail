#include "data/parser/ParserClient.h"

#include <sstream>

#include "data/parser/ParseLocation.h"

std::wstring ParserClient::addAccessPrefix(const std::wstring& str, AccessKind access)
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return L"public " + str;
	case ACCESS_PROTECTED:
		return L"protected " + str;
	case ACCESS_PRIVATE:
		return L"private " + str;
	case ACCESS_DEFAULT:
		return L"default " + str;
	default:
		break;
	}
	return str;
}

std::wstring ParserClient::addStaticPrefix(const std::wstring& str, bool isStatic)
{
	if (isStatic)
	{
		return L"static " + str;
	}
	return str;
}

std::wstring ParserClient::addConstPrefix(const std::wstring& str, bool isConst, bool atFront)
{
	if (isConst)
	{
		return atFront ? L"const " + str : str + L" const";
	}
	return str;
}

std::wstring ParserClient::addLocationSuffix(const std::wstring& str, const ParseLocation& location)
{
	std::wstringstream ss;
	ss << str;
	ss << L" <" << location.startLineNumber << L":" << location.startColumnNumber << L" ";
	ss << location.endLineNumber << L":" << location.endColumnNumber << L">";
	return ss.str();
}

std::wstring ParserClient::addLocationSuffix(
	const std::wstring& str, const ParseLocation& location, const ParseLocation& scopeLocation
) {
	if (!location.isValid())
	{
		return addLocationSuffix(str, scopeLocation);
	}
	else if (!scopeLocation.isValid())
	{
		return addLocationSuffix(str, location);
	}

	std::wstringstream ss;
	ss << str;
	ss << L" <" << scopeLocation.startLineNumber << L":" << scopeLocation.startColumnNumber;
	ss << L" <" << location.startLineNumber << L":" << location.startColumnNumber << L" ";
	ss << location.endLineNumber << L":" << location.endColumnNumber << L"> ";
	ss << scopeLocation.endLineNumber << L":" << scopeLocation.endColumnNumber << L">";
	return ss.str();
}

ParserClient::ParserClient()
	: m_hasFatalErrors(false)
{
}

void ParserClient::recordError(
	const ParseLocation& errorLocation, const std::wstring& message, bool fatal, bool indexed, const FilePath& translationUnit)
{
	doRecordError(errorLocation, message, fatal, indexed, translationUnit);

	if (fatal)
	{
		m_hasFatalErrors = true;
	}
}

bool ParserClient::hasFatalErrors() const
{
	return m_hasFatalErrors;
}
