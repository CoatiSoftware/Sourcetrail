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

	virtual void beginTraverseDecl(clang::Decl* d) override;
	virtual void endTraverseDecl(clang::Decl* d) override;

	virtual void beginTraverseStmt(clang::Stmt* s) override;
	virtual void endTraverseStmt(clang::Stmt* s) override;

	virtual void beginTraverseType(const clang::QualType& t) override;
	virtual void endTraverseType(const clang::QualType& t) override;

	virtual void beginTraverseTypeLoc(const clang::TypeLoc& tl) override;
	virtual void endTraverseTypeLoc(const clang::TypeLoc& tl) override;

	virtual void beginTraverseCallCommonCallee() override;

	virtual void beginTraverseCallCommonArgument() override;

	virtual void beginTraverseBinCommaLhs() override;

	virtual void beginTraverseBinCommaRhs() override;

	virtual void beginTraverseAssignCommonLhs() override;

	virtual void beginTraverseAssignCommonRhs() override;

	virtual void beginTraverseConstructorInitializer(clang::CXXCtorInitializer* init) override;

	virtual void beginTraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* s) override;

	virtual void beginTraverseUnresolvedMemberExpr(clang::UnresolvedMemberExpr* s) override;

	virtual void visitVarDecl(clang::VarDecl* d) override;

	virtual void visitCastExpr(clang::CastExpr* s) override;

	virtual void visitUnaryAddrOf(clang::UnaryOperator* s) override;

	virtual void visitUnaryDeref(clang::UnaryOperator* s) override;

	virtual void visitDeclStmt(clang::DeclStmt* s) override;

	virtual void visitReturnStmt(clang::ReturnStmt* s) override;

	virtual void visitInitListExpr(clang::InitListExpr* s) override;

	virtual void visitMemberExpr(clang::MemberExpr* s) override;

	virtual void visitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr* s) override;
	
private:
	void saveAll();
	void restoreAll();

	ReferenceKind m_thisRefKind;
	ReferenceKind m_childRefKind;

	std::vector<ReferenceKind> m_oldThisRefKinds;
	std::vector<ReferenceKind> m_oldChildRefKinds;
};

#endif // CXX_AST_VISITOR_COMPONENT_DECL_REF_KIND_H
