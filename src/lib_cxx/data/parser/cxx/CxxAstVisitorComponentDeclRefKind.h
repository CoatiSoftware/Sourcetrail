#ifndef CXX_AST_VISITOR_COMPONENT_DECL_REF_KIND_H
#define CXX_AST_VISITOR_COMPONENT_DECL_REF_KIND_H

#include <vector>

#include "data/parser/cxx/CxxAstVisitor.h"
#include "data/parser/cxx/CxxAstVisitorComponent.h"

#include "data/parser/ReferenceKind.h"

// This CxxAstVisitorComponent is responsible for recording and providing the context based ReferenceKind for each reference to a declaration encountered while traversing the AST.
// Example: void foo() { bar(); }
// For this snippet the reference to "bar" is used in the context of a call.
class CxxAstVisitorComponentDeclRefKind: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentDeclRefKind(CxxAstVisitor* astVisitor);
	virtual ~CxxAstVisitorComponentDeclRefKind();

	ReferenceKind getReferenceKind() const;

	virtual void beginTraverseDecl(clang::Decl* d);
	virtual void endTraverseDecl(clang::Decl* d);

	virtual void beginTraverseStmt(clang::Stmt* s);
	virtual void endTraverseStmt(clang::Stmt* s);

	virtual void beginTraverseType(clang::QualType t);
	virtual void endTraverseType(clang::QualType t);

	virtual void beginTraverseTypeLoc(clang::TypeLoc tl);
	virtual void endTraverseTypeLoc(clang::TypeLoc tl);

	virtual void beginTraverseCallCommonCallee();

	virtual void beginTraverseCallCommonArgument();

	virtual void beginTraverseBinCommaLhs();

	virtual void beginTraverseBinCommaRhs();

	virtual void beginTraverseAssignCommonLhs();

	virtual void beginTraverseAssignCommonRhs();

	virtual void beginTraverseConstructorInitializer(clang::CXXCtorInitializer* init);

	virtual void beginTraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* s);

	virtual void visitVarDecl(clang::VarDecl* d);

	virtual void visitCastExpr(clang::CastExpr* s);

	virtual void visitUnaryAddrOf(clang::UnaryOperator* s);

	virtual void visitUnaryDeref(clang::UnaryOperator* s);

	virtual void visitDeclStmt(clang::DeclStmt* s);

	virtual void visitReturnStmt(clang::ReturnStmt* s);

	virtual void visitInitListExpr(clang::InitListExpr* s);

	virtual void visitMemberExpr(clang::MemberExpr* s);

private:
	void saveAll();
	void restoreAll();

	ReferenceKind m_thisRefKind;
	ReferenceKind m_childRefKind;

	std::vector<ReferenceKind> m_oldThisRefKinds;
	std::vector<ReferenceKind> m_oldChildRefKinds;
};

#endif // CXX_AST_VISITOR_COMPONENT_DECL_REF_KIND_H
