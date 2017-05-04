#ifndef CXX_CACHE_TYPES_H
#define CXX_CACHE_TYPES_H

#include <string>
#include "utility/Cache.h"

class FilePath;

typedef Cache<std::string, FilePath> FilePathCache;

#endif // CXX_CACHE_TYPES_H
