#include "IncludeDirective.h"

IncludeDirective::IncludeDirective(
	const FilePath& includedFilePath,
	const FilePath& includingFilePath,
	unsigned int lineNumber,
	bool usesBrackets)
	: m_includedFilePath(includedFilePath)
	, m_includingFilePath(includingFilePath)
	, m_lineNumber(lineNumber)
	, m_usesBrackets(usesBrackets)
{
}

FilePath IncludeDirective::getIncludedFile() const
{
	return m_includedFilePath;
}

FilePath IncludeDirective::getIncludingFile() const
{
	return m_includingFilePath;
}

std::wstring IncludeDirective::getDirective() const
{
	return std::wstring(L"#include ") + (m_usesBrackets ? L"<" : L"\"") +
		m_includedFilePath.wstr() + (m_usesBrackets ? L">" : L"\"");
}

unsigned int IncludeDirective::getLineNumber() const
{
	return m_lineNumber;
}
