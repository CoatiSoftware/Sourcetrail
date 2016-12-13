#include "data/parser/cxx/name_resolver/CxxNameResolver.h"

CxxNameResolver::CxxNameResolver(std::vector<const clang::Decl*> ignoredContextDecls)
	: m_ignoredContextDecls(ignoredContextDecls)
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

bool CxxNameResolver::ignoresContext(const clang::DeclContext* declContext)
{
	const clang::Decl* decl = clang::dyn_cast<clang::Decl>(declContext);
	for (size_t i = 0; i < m_ignoredContextDecls.size(); i++)
	{
		if (decl == m_ignoredContextDecls[i])
		{
			return true;
		}
	}
	return false;
}

std::vector<const clang::Decl*> CxxNameResolver::getIgnoredContextDecls() const
{
	return m_ignoredContextDecls;
}
