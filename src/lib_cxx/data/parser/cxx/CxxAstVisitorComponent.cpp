#include "data/parser/cxx/CxxAstVisitorComponent.h"

CxxAstVisitorComponent::CxxAstVisitorComponent(CxxAstVisitor* astVisitor)
	: m_astVisitor(astVisitor)
{
}

CxxAstVisitorComponent::~CxxAstVisitorComponent()
{
}

CxxAstVisitor* CxxAstVisitorComponent::getAstVisitor()
{
	return m_astVisitor;
}

const CxxAstVisitor* CxxAstVisitorComponent::getAstVisitor() const
{
	return m_astVisitor;
}
