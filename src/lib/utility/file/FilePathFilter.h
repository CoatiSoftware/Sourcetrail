#ifndef FILE_PATH_FILTER_H
#define FILE_PATH_FILTER_H

#include <regex>
#include <string>

#include "utility/file/FilePath.h"

class FilePathFilter
{
public:
	explicit FilePathFilter(const std::wstring& filterString);

	std::wstring wstr() const;

	bool isMatching(const FilePath& filePath) const;

	bool operator<(const FilePathFilter& other) const;

private:
	static std::wregex convertFilterStringToRegex(const std::wstring& filterString);

	std::wstring m_filterString;
	std::wregex m_filterRegex;
};

#endif // FILE_PATH_FILTER_H
