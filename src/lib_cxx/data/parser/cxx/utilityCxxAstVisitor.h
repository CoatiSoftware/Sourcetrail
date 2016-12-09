#ifndef UTILITY_CXX_AST_VISITOR_H
#define UTILITY_CXX_AST_VISITOR_H

#include <clang/AST/Decl.h>

#include "data/parser/AccessKind.h"
#include "data/parser/SymbolKind.h"

namespace utility
{
	bool isImplicit(const clang::Decl* d);
	AccessKind convertAccessSpecifier(clang::AccessSpecifier access);
	SymbolKind convertTagKind(clang::TagTypeKind tagKind);
}

#endif // UTILITY_CXX_AST_VISITOR_H
