#ifndef CXX_AST_VISITOR_COMPONENT_TYPE_REF_KIND_H
#define CXX_AST_VISITOR_COMPONENT_TYPE_REF_KIND_H

#include <vector>

#include "CxxAstVisitor.h"
#include "CxxAstVisitorComponent.h"

#include "ReferenceKind.h"

// This CxxAstVisitorComponent is responsible for recording and providing the context based ReferenceKind for each reference to a type encountered while traversing the AST.
// Example: class Foo: public Bar {};
// For this snippet the type "Bar" is used in the context of an inheritence.
class CxxAstVisitorComponentTypeRefKind: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentTypeRefKind(CxxAstVisitor* astVisitor);

	ReferenceKind getReferenceKind() const;

	void beginTraverseCXXBaseSpecifier() override;
	void endTraverseCXXBaseSpecifier() override;

	void beginTraverseTemplateDefaultArgumentLoc() override;
	void endTraverseTemplateDefaultArgumentLoc() override;

	void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;
	void endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;

private:
	std::vector<ReferenceKind> m_refKindStack;
};

#endif // CXX_AST_VISITOR_COMPONENT_TYPE_REF_KIND_H
