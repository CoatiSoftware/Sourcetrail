#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <clang/AST/Decl.h>

#include "data/parser/AccessKind.h"
#include "data/parser/SymbolKind.h"

namespace utility
{
	template <typename T>
	const T* getFirstDecl(const T* decl);
	bool isImplicit(const clang::Decl* d);
	AccessKind convertAccessSpecifier(clang::AccessSpecifier access);
	SymbolKind convertTagKind(const clang::TagTypeKind tagKind);
	SymbolKind getSymbolKind(const clang::VarDecl* d);
	std::wstring getFileNameOfFileEntry(const clang::FileEntry* entry);
}

template <typename T>
const T* utility::getFirstDecl(const T* decl)
{
	const clang::Decl* ret = decl;
	{
		const clang::Decl* prev = ret;
		while (prev)
		{
			ret = prev;
			prev = prev->getPreviousDecl();
		}
	}
	return clang::dyn_cast_or_null<T>(ret);
}

#endif // UTILITY_CLANG_H
