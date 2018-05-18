#ifndef UTILITY_FILE_H
#define UTILITY_FILE_H

#include <vector>
#include <set>

class FilePath;

namespace utility
{
	std::vector<FilePath> getTopLevelPaths(const std::vector<FilePath>& paths);
	std::vector<FilePath> getTopLevelPaths(const std::set<FilePath>& paths);
}

#endif // UTILITY_FILE_H
