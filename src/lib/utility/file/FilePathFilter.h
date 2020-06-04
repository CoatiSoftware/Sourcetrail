#ifndef FILE_PATH_FILTER_H
#define FILE_PATH_FILTER_H

#include <regex>
#include <string>

#include "FilePath.h"

class FilePathFilter
{
public:
	template<typename ContainerType>
	static bool areMatching(const ContainerType& filters, const FilePath& filePath);

	explicit FilePathFilter(const std::wstring& filterString);

	std::wstring wstr() const;

	bool isMatching(const FilePath& filePath) const;
	bool isMatching(const std::wstring& fileStr) const;

	bool operator<(const FilePathFilter& other) const;

private:
	static std::wregex convertFilterStringToRegex(const std::wstring& filterString);

	std::wstring m_filterString;
	std::wregex m_filterRegex;
};

template<typename ContainerType>
bool FilePathFilter::areMatching(const ContainerType& filters, const FilePath& filePath)
{
	const std::wstring fileStr = filePath.wstr();

	for (const FilePathFilter& filter: filters)
	{
		if (filter.isMatching(fileStr))
		{
			return true;
		}
	}

	return false;
}

#endif	  // FILE_PATH_FILTER_H
