#include "data/parser/cxx/CxxAstVisitorComponentTypeRefKind.h"

CxxAstVisitorComponentTypeRefKind::CxxAstVisitorComponentTypeRefKind(CxxAstVisitor* astVisitor)
	: CxxAstVisitorComponent(astVisitor)
{
}

ReferenceKind CxxAstVisitorComponentTypeRefKind::getReferenceKind() const
{
	for (auto it = m_refKindStack.rbegin(); it != m_refKindStack.rend(); it++)
	{
		if ((*it) != REFERENCE_UNDEFINED)
		{
			return (*it);
		}
	}
	return REFERENCE_TYPE_USAGE;
}

void CxxAstVisitorComponentTypeRefKind::beginTraverseCXXBaseSpecifier()
{
	m_refKindStack.push_back(REFERENCE_INHERITANCE);
}

void CxxAstVisitorComponentTypeRefKind::endTraverseCXXBaseSpecifier()
{
	m_refKindStack.pop_back();
}

void CxxAstVisitorComponentTypeRefKind::beginTraverseTemplateDefaultArgumentLoc()
{
	m_refKindStack.push_back(REFERENCE_TEMPLATE_DEFAULT_ARGUMENT);
}

void CxxAstVisitorComponentTypeRefKind::endTraverseTemplateDefaultArgumentLoc()
{
	m_refKindStack.pop_back();
}

void CxxAstVisitorComponentTypeRefKind::beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	if (getReferenceKind() != REFERENCE_TEMPLATE_DEFAULT_ARGUMENT)
	{
		m_refKindStack.push_back(REFERENCE_TEMPLATE_ARGUMENT);
	}
	else
	{
		m_refKindStack.push_back(REFERENCE_UNDEFINED);
	}
}

void CxxAstVisitorComponentTypeRefKind::endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc)
{
	m_refKindStack.pop_back();
}
