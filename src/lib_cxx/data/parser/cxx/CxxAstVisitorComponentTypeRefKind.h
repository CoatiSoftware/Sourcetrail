#ifndef CXX_AST_VISITOR_COMPONENT_TYPE_REF_KIND_H
#define CXX_AST_VISITOR_COMPONENT_TYPE_REF_KIND_H

#include <vector>

#include "data/parser/cxx/CxxAstVisitor.h"
#include "data/parser/cxx/CxxAstVisitorComponent.h"

#include "data/parser/ReferenceKind.h"

// This CxxAstVisitorComponent is responsible for recording and providing the context based ReferenceKind for each reference to a type encountered while traversing the AST.
// Example: class Foo: public Bar {};
// For this snippet the type "Bar" is used in the context of an inheritence.
class CxxAstVisitorComponentTypeRefKind: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentTypeRefKind(CxxAstVisitor* astVisitor);
	virtual ~CxxAstVisitorComponentTypeRefKind();

	ReferenceKind getReferenceKind() const;

	virtual void beginTraverseCXXBaseSpecifier();
	virtual void endTraverseCXXBaseSpecifier();

	virtual void beginTraverseTemplateDefaultArgumentLoc();
	virtual void endTraverseTemplateDefaultArgumentLoc();

	virtual void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);
	virtual void endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);

private:
	std::vector<ReferenceKind> m_refKindStack;
};

#endif // CXX_AST_VISITOR_COMPONENT_TYPE_REF_KIND_H
