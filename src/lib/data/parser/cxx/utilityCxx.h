#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <memory>
#include <string>
#include <vector>

#include "clang/AST/Type.h"
#include "clang/AST/Decl.h"

class DataType;

namespace utility
{
	DataType qualTypeToDataType(clang::QualType qualType);

	std::vector<std::string> getDeclNameHierarchy(clang::Decl* declaration);
	std::vector<std::string> getContextNameHierarchy(clang::DeclContext* declaration);
	std::string getDeclName(clang::NamedDecl* declaration);
}

#endif // UTILITY_CLANG_H
