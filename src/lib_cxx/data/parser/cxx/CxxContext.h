#ifndef CXX_CONTEXT_H
#define CXX_CONTEXT_H

#include <clang/AST/Decl.h>

#include "NameHierarchy.h"
#include "OrderedCache.h"

typedef OrderedCache<const clang::NamedDecl*, NameHierarchy> DeclNameCache;
typedef OrderedCache<const clang::Type*, NameHierarchy> TypeNameCache;

class CxxContext
{
public:
	virtual ~CxxContext() = default;
	virtual NameHierarchy getName() = 0;
	virtual const clang::NamedDecl* getDecl() = 0;
};


class CxxContextDecl
	: public CxxContext
{
public:
	CxxContextDecl(const clang::NamedDecl* decl, DeclNameCache* nameCache);
	NameHierarchy getName() override;
	const clang::NamedDecl* getDecl() override;

private:
	const clang::NamedDecl* m_decl;
	DeclNameCache* m_nameCache;
};


class CxxContextType
	: public CxxContext
{
public:
	CxxContextType(const clang::Type* type, TypeNameCache* nameCache);
	NameHierarchy getName() override;
	const clang::NamedDecl* getDecl() override;

private:
	const clang::Type* m_type;
	TypeNameCache* m_nameCache;
};

#endif // CXX_CONTEXT_H

