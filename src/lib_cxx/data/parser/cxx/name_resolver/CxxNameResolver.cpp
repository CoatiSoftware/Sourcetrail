#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

CxxNameResolver::CxxNameResolver(
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
	std::vector<const clang::Decl*> ignoredContextDecls
)
	: m_canonicalFilePathCache(canonicalFilePathCache)
	, m_ignoredContextDecls(ignoredContextDecls)
{
}

CxxNameResolver::~CxxNameResolver()
{
}

void CxxNameResolver::ignoreContextDecl(const clang::Decl* decl)
{
	if (decl)
	{
		m_ignoredContextDecls.push_back(decl);
	}
}

bool CxxNameResolver::ignoresContext(const clang::Decl* decl)
{
	if (decl)
	{
		for (size_t i = 0; i < m_ignoredContextDecls.size(); i++)
		{
			if (decl == m_ignoredContextDecls[i])
			{
				return true;
			}
		}
	}
	return false;
}

bool CxxNameResolver::ignoresContext(const clang::DeclContext* declContext)
{
	if (const clang::Decl* decl = clang::dyn_cast_or_null<clang::Decl>(declContext))
	{
		return ignoresContext(decl);
	}
	return false;
}

std::shared_ptr<CanonicalFilePathCache> CxxNameResolver::getCanonicalFilePathCache() const
{
	return m_canonicalFilePathCache;
}

std::vector<const clang::Decl*> CxxNameResolver::getIgnoredContextDecls() const
{
	return m_ignoredContextDecls;
}
