#ifndef CXX_AST_VISITOR_COMPONENT_IMPLICIT_CODE_H
#define CXX_AST_VISITOR_COMPONENT_IMPLICIT_CODE_H

#include "CxxAstVisitorComponent.h"

// This CxxAstVisitorComponent is responsible for deciding if the AstVisitor should visit implicit code in the current context.
class CxxAstVisitorComponentImplicitCode
	: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentImplicitCode(CxxAstVisitor* astVisitor);

	bool shouldVisitImplicitCode() const;

	void beginTraverseDecl(clang::Decl* d);
	void endTraverseDecl(clang::Decl* d);

	void beginTraverseCXXForRangeStmt(clang::CXXForRangeStmt* s);
	void endTraverseCXXForRangeStmt(clang::CXXForRangeStmt* s);

private:
	std::vector<bool> m_stack;
};

#endif // CXX_AST_VISITOR_COMPONENT_IMPLICIT_CODE_H
