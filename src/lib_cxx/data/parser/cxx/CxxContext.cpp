#include "CxxContext.h"

CxxContextDecl::CxxContextDecl(const clang::NamedDecl* decl, DeclNameCache* nameCache)
	: m_decl(decl)
	, m_nameCache(nameCache)
{
}

NameHierarchy CxxContextDecl::getName()
{
	return m_nameCache->getValue(m_decl);
}

const clang::NamedDecl* CxxContextDecl::getDecl()
{
	return m_decl;
}


CxxContextType::CxxContextType(const clang::Type* type, TypeNameCache* nameCache)
	: m_type(type)
	, m_nameCache(nameCache)
{
}

NameHierarchy CxxContextType::getName()
{
	return m_nameCache->getValue(m_type);
}

const clang::NamedDecl* CxxContextType::getDecl()
{
	return nullptr;
}
