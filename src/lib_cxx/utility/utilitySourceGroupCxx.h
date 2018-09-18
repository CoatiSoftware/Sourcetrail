#ifndef UTILITY_SOURCE_GROUP_CXX_H
#define UTILITY_SOURCE_GROUP_CXX_H

#include <set>
#include <vector>

#include "FilePath.h"
#include "FilePathFilter.h"

namespace utility
{
	std::set<FilePath> filterToContainedFilePaths(
		const std::set<FilePath>& filePaths,
		const std::set<FilePath>& indexedFilePaths,
		const std::set<FilePath>& indexedFileOrDirectoryPaths,
		const std::vector<FilePathFilter>& excludeFilters);
}

#endif // UTILITY_SOURCE_GROUP_CXX_H
