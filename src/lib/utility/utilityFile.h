#ifndef UTILITY_FILE_H
#define UTILITY_FILE_H

#include <vector>

#include "utility/file/FilePath.h"

namespace utility
{
	std::vector<FilePath> getTopLevelPaths(const std::vector<FilePath>& paths);
}

#endif // UTILITY_FILE_H
