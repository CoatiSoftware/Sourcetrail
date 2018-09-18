#ifndef CXX_CONTEXT_H
#define CXX_CONTEXT_H

#include <clang/AST/Decl.h>

#include "NameHierarchy.h"
#include "UnorderedCache.h"

typedef UnorderedCache<const clang::NamedDecl*, NameHierarchy> DeclNameCache;
typedef UnorderedCache<const clang::Type*, NameHierarchy> TypeNameCache;

class CxxContext
{
public:
	virtual ~CxxContext() = default;
	virtual NameHierarchy getName() = 0;
	virtual const clang::NamedDecl* getDecl() = 0;
};

class CxxContextDecl: public CxxContext
{
public:
	CxxContextDecl(const clang::NamedDecl* decl, std::shared_ptr<DeclNameCache> nameCache);
	NameHierarchy getName() override;
	const clang::NamedDecl* getDecl() override;

private:
	const clang::NamedDecl* m_decl;
	std::shared_ptr<DeclNameCache> m_nameCache;
};

class CxxContextType: public CxxContext
{
public:
	CxxContextType(const clang::Type* type, std::shared_ptr<TypeNameCache> nameCache);
	NameHierarchy getName() override;
	const clang::NamedDecl* getDecl() override;

private:
	const clang::Type* m_type;
	std::shared_ptr<TypeNameCache> m_nameCache;
};

#endif // CXX_CONTEXT_H

