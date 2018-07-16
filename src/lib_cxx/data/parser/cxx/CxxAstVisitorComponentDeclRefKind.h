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

	ReferenceKind getReferenceKind() const;

	void beginTraverseDecl(clang::Decl* d) override;
	void endTraverseDecl(clang::Decl* d) override;

	void beginTraverseStmt(clang::Stmt* s) override;
	void endTraverseStmt(clang::Stmt* s) override;

	void beginTraverseType(const clang::QualType& t) override;
	void endTraverseType(const clang::QualType& t) override;

	void beginTraverseTypeLoc(const clang::TypeLoc& tl) override;
	void endTraverseTypeLoc(const clang::TypeLoc& tl) override;

	void beginTraverseCallCommonCallee() override;

	void beginTraverseCallCommonArgument() override;

	void beginTraverseBinCommaLhs() override;

	void beginTraverseBinCommaRhs() override;

	void beginTraverseAssignCommonLhs() override;

	void beginTraverseAssignCommonRhs() override;

	void beginTraverseConstructorInitializer(clang::CXXCtorInitializer* init) override;

	void beginTraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* s) override;

	void beginTraverseUnresolvedMemberExpr(clang::UnresolvedMemberExpr* s) override;

	void visitVarDecl(clang::VarDecl* d) override;

	void visitCastExpr(clang::CastExpr* s) override;

	void visitUnaryAddrOf(clang::UnaryOperator* s) override;

	void visitUnaryDeref(clang::UnaryOperator* s) override;

	void visitDeclStmt(clang::DeclStmt* s) override;

	void visitReturnStmt(clang::ReturnStmt* s) override;

	void visitInitListExpr(clang::InitListExpr* s) override;

	void visitMemberExpr(clang::MemberExpr* s) override;

	void visitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr* s) override;

private:
	void saveAll();
	void restoreAll();

	ReferenceKind m_thisRefKind;
	ReferenceKind m_childRefKind;

	std::vector<ReferenceKind> m_oldThisRefKinds;
	std::vector<ReferenceKind> m_oldChildRefKinds;
};

#endif // CXX_AST_VISITOR_COMPONENT_DECL_REF_KIND_H
