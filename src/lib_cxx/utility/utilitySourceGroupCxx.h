#ifndef UTILITY_SOURCE_GROUP_CXX_H
#define UTILITY_SOURCE_GROUP_CXX_H

#include <set>
#include <vector>

#include "utility/file/FilePath.h"
#include "utility/file/FilePathFilter.h"

namespace utility
{
	std::set<FilePath> filterToContainedFilePaths(
		const std::set<FilePath> filePaths,
		const std::set<FilePath>& indexedSourcePaths,
		const std::set<FilePath>& indexedHeaderPaths,
		const std::vector<FilePathFilter> excludeFilters);
}

#endif // UTILITY_SOURCE_GROUP_CXX_H
