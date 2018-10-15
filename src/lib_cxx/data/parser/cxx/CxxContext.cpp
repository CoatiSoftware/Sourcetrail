#include "CxxContext.h"

const clang::NamedDecl* CxxContext::getDecl() const
{
	return nullptr;
}

const clang::Type* CxxContext::getType() const
{
	return nullptr;
}


CxxContextDecl::CxxContextDecl(const clang::NamedDecl* decl)
	: m_decl(decl)
{
}

const clang::NamedDecl* CxxContextDecl::getDecl() const
{
	return m_decl;
}


CxxContextType::CxxContextType(const clang::Type* type)
	: m_type(type)
{
}

const clang::Type* CxxContextType::getType() const
{
	return m_type;
}
