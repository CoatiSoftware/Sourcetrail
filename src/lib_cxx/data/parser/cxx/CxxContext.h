#ifndef CXX_CONTEXT_H
#define CXX_CONTEXT_H

#include <clang/AST/Decl.h>

#include "data/name/NameHierarchy.h"
#include "utility/Cache.h"

typedef Cache<const clang::NamedDecl*, NameHierarchy> DeclNameCache;
typedef Cache<const clang::Type*, NameHierarchy> TypeNameCache;

class CxxContext
{
public:
	virtual ~CxxContext();
	virtual NameHierarchy getName() = 0;
};

class CxxContextDecl: public CxxContext
{
public:
	CxxContextDecl(const clang::NamedDecl* decl, std::shared_ptr<DeclNameCache> nameCache);
	virtual ~CxxContextDecl();
	virtual NameHierarchy getName();
	const clang::NamedDecl* getDecl();

private:
	const clang::NamedDecl* m_decl;
	std::shared_ptr<DeclNameCache> m_nameCache;
};

class CxxContextType: public CxxContext
{
public:
	CxxContextType(const clang::Type* type, std::shared_ptr<TypeNameCache> nameCache);
	virtual ~CxxContextType();
	virtual NameHierarchy getName();

private:
	const clang::Type* m_type;
	std::shared_ptr<TypeNameCache> m_nameCache;
};

#endif // CXX_CONTEXT_H

