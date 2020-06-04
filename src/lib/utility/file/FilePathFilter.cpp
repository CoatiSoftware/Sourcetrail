#include "FilePathFilter.h"

FilePathFilter::FilePathFilter(const std::wstring& filterString)
	: m_filterString(filterString), m_filterRegex(convertFilterStringToRegex(filterString))
{
}

std::wstring FilePathFilter::wstr() const
{
	return m_filterString;
}

bool FilePathFilter::isMatching(const FilePath& filePath) const
{
	return isMatching(filePath.wstr());
}

bool FilePathFilter::isMatching(const std::wstring& fileStr) const
{
	return std::regex_match(fileStr, m_filterRegex);
}

bool FilePathFilter::operator<(const FilePathFilter& other) const
{
	return m_filterString.compare(other.m_filterString) < 0;
}

std::wregex FilePathFilter::convertFilterStringToRegex(const std::wstring& filterString)
{
	std::wstring regexFilterString = filterString;

	{
		std::wregex regex(L"[\\\\/]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\\\/]");
	}

	{
		std::wregex regex(L"([^\\\\])([^/])([\\]])");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"$1$2[\\]]");
	}

	{
		std::wregex regex(L"([\\[])([^\\\\])");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\[]$2");
	}

	{
		std::wregex regex(L"[\\(]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\(]");
	}

	{
		std::wregex regex(L"[\\)]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\)]");
	}

	{
		std::wregex regex(L"[\\{]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\{]");
	}

	{
		std::wregex regex(L"[\\}]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\}]");
	}

	{
		std::wregex regex(L"[\\+]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\+]");
	}

	{
		std::wregex regex(L"[\\-]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\-]");
	}

	{
		std::wregex regex(L"[\\$]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\$]");
	}

	{
		std::wregex regex(L"[\\.]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\.]");
	}

	{
		std::wregex regex(L"[\\^]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[\\^]");
	}

	{
		std::wregex regex(L"[\\*][\\*]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L".{0,}");
	}

	{
		std::wregex regex(L"[\\*]");
		regexFilterString = std::regex_replace(regexFilterString, regex, L"[^\\\\/]*");
	}

	return std::wregex(regexFilterString, std::regex::optimize);
}
