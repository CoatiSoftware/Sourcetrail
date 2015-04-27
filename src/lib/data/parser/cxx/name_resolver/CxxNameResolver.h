#ifndef CXX_NAME_RESOLVER_H
#define CXX_NAME_RESOLVER_H

#include <vector>

#include "clang/AST/Decl.h"

class CxxNameResolver
{
public:
	CxxNameResolver(std::vector<const clang::Decl*> ignoredContextDecls);
	~CxxNameResolver();

	void ignoreContextDecl(const clang::Decl* decl);
	bool ignoresContext(const clang::DeclContext* declContext);

protected:
	std::vector<const clang::Decl*> getIgnoredContextDecls() const;

private:
	std::vector<const clang::Decl*> m_ignoredContextDecls;
};

#endif // CXX_NAME_RESOLVER_H
