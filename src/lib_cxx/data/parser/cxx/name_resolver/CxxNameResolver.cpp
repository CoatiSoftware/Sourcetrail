#include "CxxNameResolver.h"

CxxNameResolver::CxxNameResolver(CanonicalFilePathCache* canonicalFilePathCache)
	: m_canonicalFilePathCache(canonicalFilePathCache)
{
}

CxxNameResolver::CxxNameResolver(const CxxNameResolver* other)
	: m_canonicalFilePathCache(other->getCanonicalFilePathCache())
	, m_ignoredContextDecls(other->getIgnoredContextDecls())
{
}

void CxxNameResolver::ignoreContextDecl(const clang::Decl* decl)
{
	if (decl)
	{
		m_ignoredContextDecls.emplace_back(decl);
	}
}

bool CxxNameResolver::ignoresContext(const clang::Decl* decl) const
{
	if (decl)
	{
		for (const clang::Decl* ignoredDecl: m_ignoredContextDecls)
		{
			if (decl == ignoredDecl)
			{
				return true;
			}
		}
	}
	return false;
}

bool CxxNameResolver::ignoresContext(const clang::DeclContext* declContext) const
{
	if (const clang::Decl* decl = clang::dyn_cast_or_null<clang::Decl>(declContext))
	{
		return ignoresContext(decl);
	}
	return false;
}

CanonicalFilePathCache* CxxNameResolver::getCanonicalFilePathCache() const
{
	return m_canonicalFilePathCache;
}

const std::vector<const clang::Decl*>& CxxNameResolver::getIgnoredContextDecls() const
{
	return m_ignoredContextDecls;
}
