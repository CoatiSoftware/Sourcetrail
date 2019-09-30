#include "CxxAstVisitorComponentTypeRefKind.h"

CxxAstVisitorComponentTypeRefKind::CxxAstVisitorComponentTypeRefKind(CxxAstVisitor* astVisitor)
	: CxxAstVisitorComponent(astVisitor)
{
}

bool CxxAstVisitorComponentTypeRefKind::isTraversingInheritance() const
{
	return (!m_stateKindStack.empty() && m_stateKindStack.back() == STATE_INHERITANCE);
}

bool CxxAstVisitorComponentTypeRefKind::isTraversingTemplateArgument() const
{
	return (!m_stateKindStack.empty() && m_stateKindStack.back() == STATE_TEMPLATE_ARGUMENT);
}

void CxxAstVisitorComponentTypeRefKind::beginTraverseCXXBaseSpecifier()
{
	m_stateKindStack.push_back(STATE_INHERITANCE);
}

void CxxAstVisitorComponentTypeRefKind::endTraverseCXXBaseSpecifier()
{
	m_stateKindStack.pop_back();
}

void CxxAstVisitorComponentTypeRefKind::beginTraverseTemplateDefaultArgumentLoc()
{
	m_stateKindStack.push_back(STATE_USAGE);
}

void CxxAstVisitorComponentTypeRefKind::endTraverseTemplateDefaultArgumentLoc()
{
	m_stateKindStack.pop_back();
}

void CxxAstVisitorComponentTypeRefKind::beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	m_stateKindStack.push_back(STATE_TEMPLATE_ARGUMENT);
}

void CxxAstVisitorComponentTypeRefKind::endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	m_stateKindStack.pop_back();
}
