#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <clang/AST/Decl.h>

#include "data/parser/AccessKind.h"
#include "data/parser/SymbolKind.h"

namespace utility
{
	bool isImplicit(const clang::Decl* d);
	AccessKind convertAccessSpecifier(clang::AccessSpecifier access);
	SymbolKind convertTagKind(const clang::TagTypeKind tagKind);
	SymbolKind getSymbolKind(const clang::VarDecl* d);
	std::string getFileNameOfFileEntry(const clang::FileEntry* entry);
}

#endif // UTILITY_CLANG_H
