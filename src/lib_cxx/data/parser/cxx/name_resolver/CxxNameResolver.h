#ifndef CXX_NAME_RESOLVER_H
#define CXX_NAME_RESOLVER_H

#include <vector>

#include <clang/AST/Decl.h>

class CanonicalFilePathCache;

class CxxNameResolver
{
public:
	CxxNameResolver(
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::vector<const clang::Decl*> ignoredContextDecls
	);
	virtual ~CxxNameResolver();

	void ignoreContextDecl(const clang::Decl* decl);
	bool ignoresContext(const clang::Decl* decl);
	bool ignoresContext(const clang::DeclContext* declContext);

protected:
	std::shared_ptr<CanonicalFilePathCache> getCanonicalFilePathCache() const;
	std::vector<const clang::Decl*> getIgnoredContextDecls() const;

private:
	std::shared_ptr<CanonicalFilePathCache> m_canonicalFilePathCache;
	std::vector<const clang::Decl*> m_ignoredContextDecls;
};

#endif // CXX_NAME_RESOLVER_H
