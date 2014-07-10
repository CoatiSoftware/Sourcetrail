#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <memory>

#include "clang/AST/Type.h"

class DataType;

namespace utility
{
	DataType qualTypeToDataType(clang::QualType qualType);
}

#endif // UTILITY_CLANG_H
