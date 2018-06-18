#ifndef UTILITY_CXX_H
#define UTILITY_CXX_H

#include <vector>

#include <utility/file/FilePath.h>

namespace utility
{
	std::vector<FilePath> replaceOrAddCxxCompilerHeaderPath(const std::vector<FilePath>& headerSearchPaths);
}

#endif // UTILITY_CXX_H
