#ifndef CXX_NAME_RESOLVER_H
#define CXX_NAME_RESOLVER_H

#include <vector>

#include <clang/AST/Decl.h>

class CanonicalFilePathCache;

class CxxNameResolver
{
public:
	CxxNameResolver(CanonicalFilePathCache* canonicalFilePathCache);
	CxxNameResolver(const CxxNameResolver* other);

	void ignoreContextDecl(const clang::Decl* decl);
	bool ignoresContext(const clang::Decl* decl) const;
	bool ignoresContext(const clang::DeclContext* declContext) const;

protected:
	CanonicalFilePathCache* getCanonicalFilePathCache() const;
	const std::vector<const clang::Decl*>& getIgnoredContextDecls() const;

private:
	CanonicalFilePathCache* m_canonicalFilePathCache;
	std::vector<const clang::Decl*> m_ignoredContextDecls;
};

#endif // CXX_NAME_RESOLVER_H
