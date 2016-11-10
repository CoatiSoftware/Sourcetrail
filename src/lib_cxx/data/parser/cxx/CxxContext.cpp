#include "data/parser/cxx/CxxContext.h"

CxxContext::~CxxContext()
{
}

CxxContextDecl::CxxContextDecl(const clang::NamedDecl* decl, std::shared_ptr<DeclNameCache> nameCache)
	: m_decl(decl)
	, m_nameCache(nameCache)
{
}

CxxContextDecl::~CxxContextDecl()
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

CxxContextType::CxxContextType(const clang::Type* type, std::shared_ptr<TypeNameCache> nameCache)
	: m_type(type)
	, m_nameCache(nameCache)
{
}

CxxContextType::~CxxContextType()
{
}

NameHierarchy CxxContextType::getName()
{
	return m_nameCache->getValue(m_type);
}
