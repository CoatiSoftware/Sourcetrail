#ifndef UTILITY_CLANG_H
#define UTILITY_CLANG_H

#include <clang/AST/Decl.h>

#include "AccessKind.h"
#include "SymbolKind.h"

struct ParseLocation;
struct ParseLocation;
class CanonicalFilePathCache;
class FilePath;

namespace clang
{
class SourceRange;
class Preprocessor;
class SourceManager;
}	 // namespace clang

namespace utility
{
template <typename T>
const T* getFirstDecl(const T* decl);
bool isImplicit(const clang::Decl* d);
AccessKind convertAccessSpecifier(clang::AccessSpecifier access);
SymbolKind convertTagKind(const clang::TagTypeKind tagKind);
bool isLocalVariable(const clang::VarDecl* d);
bool isParameter(const clang::VarDecl* d);
SymbolKind getSymbolKind(const clang::VarDecl* d);
std::wstring getFileNameOfFileEntry(const clang::FileEntry* entry);

ParseLocation getParseLocation(
	const clang::SourceLocation& sourceLocation,
	const clang::SourceManager& sourceManager,
	clang::Preprocessor* preprocessor,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);

ParseLocation getParseLocation(
	const clang::SourceRange& sourceRange,
	const clang::SourceManager& sourceManager,
	clang::Preprocessor* preprocessor,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache);
}	 // namespace utility

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

#endif	  // UTILITY_CLANG_H
