#ifndef CXX_AST_VISITOR_COMPONENT_IMPLICIT_CODE_H
#define CXX_AST_VISITOR_COMPONENT_IMPLICIT_CODE_H

#include "data/parser/cxx/CxxAstVisitorComponent.h"

// This CxxAstVisitorComponent is responsible for deciding if the AstVisitor should visit implicit code in the current context.
class CxxAstVisitorComponentImplicitCode: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentImplicitCode(CxxAstVisitor* astVisitor);

	bool shouldVisitImplicitCode() const;

	void beginTraverseDecl(clang::Decl* d) override;
	void endTraverseDecl(clang::Decl* d) override;

	void beginTraverseCXXForRangeStmt(clang::CXXForRangeStmt* s) override;
	void endTraverseCXXForRangeStmt(clang::CXXForRangeStmt* s) override;

private:
	std::vector<bool> m_stack;
};

#endif // CXX_AST_VISITOR_COMPONENT_IMPLICIT_CODE_H
