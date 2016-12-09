#ifndef CXX_AST_VISITOR_COMPONENT_H
#define CXX_AST_VISITOR_COMPONENT_H

#include "data/parser/cxx/CxxAstVisitor.h"

// CxxAstVisitorComponent: This is the base class for all ast visitor components.
// Each component can override it's begin-/endTraverse and visit methods in order to provide some functionality. The CxxAstVisitor
// executes all of these methods of registered components while traversing the AST.
class CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponent(CxxAstVisitor* astVisitor);
	virtual ~CxxAstVisitorComponent();

#define DEF_TRAVERSE_CUSTOM_TYPE_PTR(__NAME_TYPE__, __PARAM_TYPE__)				\
	virtual void beginTraverse##__NAME_TYPE__(clang::__PARAM_TYPE__ *v) {}		\
	virtual void endTraverse##__NAME_TYPE__(clang::__PARAM_TYPE__ *v) {}

#define DEF_TRAVERSE_CUSTOM_TYPE(__NAME_TYPE__, __PARAM_TYPE__)					\
	virtual void beginTraverse##__NAME_TYPE__(clang::__PARAM_TYPE__ v) {}		\
	virtual void endTraverse##__NAME_TYPE__(clang::__PARAM_TYPE__ v) {}

#define DEF_TRAVERSE_TYPE_PTR(__TYPE__)											\
	DEF_TRAVERSE_CUSTOM_TYPE_PTR(__TYPE__, __TYPE__)

#define DEF_TRAVERSE_TYPE(__TYPE__)												\
	DEF_TRAVERSE_CUSTOM_TYPE(__TYPE__, __TYPE__)

DEF_TRAVERSE_TYPE_PTR(Decl)

DEF_TRAVERSE_TYPE_PTR(Stmt)

DEF_TRAVERSE_CUSTOM_TYPE(Type, QualType)

DEF_TRAVERSE_TYPE(TypeLoc)

DEF_TRAVERSE_TYPE_PTR(FunctionDecl)

DEF_TRAVERSE_TYPE_PTR(ClassTemplateSpecializationDecl)

DEF_TRAVERSE_TYPE_PTR(ClassTemplatePartialSpecializationDecl)

DEF_TRAVERSE_TYPE(TemplateSpecializationTypeLoc)

DEF_TRAVERSE_TYPE_PTR(LambdaExpr)

DEF_TRAVERSE_TYPE_PTR(DeclRefExpr)

DEF_TRAVERSE_TYPE_PTR(UnresolvedLookupExpr)

	virtual void beginTraverseCallCommonCallee() {}
	virtual void endTraverseCallCommonCallee() {}

	virtual void beginTraverseCallCommonArgument() {}
	virtual void endTraverseCallCommonArgument() {}

	virtual void beginTraverseBinCommaLhs() {}
	virtual void endTraverseBinCommaLhs() {}

	virtual void beginTraverseBinCommaRhs() {}
	virtual void endTraverseBinCommaRhs() {}

	virtual void beginTraverseAssignCommonLhs() {}
	virtual void endTraverseAssignCommonLhs() {}

	virtual void beginTraverseAssignCommonRhs() {}
	virtual void endTraverseAssignCommonRhs() {}

	virtual void beginTraverseCXXBaseSpecifier() {}
	virtual void endTraverseCXXBaseSpecifier() {}

	virtual void beginTraverseTemplateDefaultArgumentLoc() {}
	virtual void endTraverseTemplateDefaultArgumentLoc() {}

DEF_TRAVERSE_CUSTOM_TYPE_PTR(ConstructorInitializer, CXXCtorInitializer)

DEF_TRAVERSE_TYPE_PTR(CXXTemporaryObjectExpr)

	virtual void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) {}
	virtual void endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) {}

	virtual void beginTraverseLambdaCapture(clang::LambdaExpr *lambdaExpr, const clang::LambdaCapture *capture) {}
	virtual void endTraverseLambdaCapture(clang::LambdaExpr *lambdaExpr, const clang::LambdaCapture *capture) {}

	virtual void visitTagDecl(clang::TagDecl* d) {}
	virtual void visitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d) {}
	virtual void visitVarDecl(clang::VarDecl* d) {}
	virtual void visitFieldDecl(clang::FieldDecl* d) {}
	virtual void visitFunctionDecl(clang::FunctionDecl* d) {}
	virtual void visitCXXMethodDecl(clang::CXXMethodDecl* d) {}
	virtual void visitEnumConstantDecl(clang::EnumConstantDecl* d) {}
	virtual void visitNamespaceDecl(clang::NamespaceDecl* d) {}
	virtual void visitNamespaceAliasDecl(clang::NamespaceAliasDecl* d) {}
	virtual void visitTypedefDecl(clang::TypedefDecl* d) {}
	virtual void visitTypeAliasDecl(clang::TypeAliasDecl* d) {}
	virtual void visitUsingDirectiveDecl(clang::UsingDirectiveDecl* d) {}
	virtual void visitUsingDecl(clang::UsingDecl* d) {}
	virtual void visitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d) {}
	virtual void visitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d) {}
	virtual void visitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d) {}

	virtual void visitTypeLoc(clang::TypeLoc tl) {}

	virtual void visitCastExpr(clang::CastExpr* s) {}
	virtual void visitUnaryAddrOf(clang::UnaryOperator* s) {}
	virtual void visitUnaryDeref(clang::UnaryOperator* s) {}
	virtual void visitDeclStmt(clang::DeclStmt* s) {}
	virtual void visitReturnStmt(clang::ReturnStmt* s) {}
	virtual void visitInitListExpr(clang::InitListExpr* s) {}
	virtual void visitDeclRefExpr(clang::DeclRefExpr* s) {}
	virtual void visitMemberExpr(clang::MemberExpr* s) {}
	virtual void visitCXXConstructExpr(clang::CXXConstructExpr* s) {}
	virtual void visitLambdaExpr(clang::LambdaExpr* s) {}

	virtual void visitConstructorInitializer(clang::CXXCtorInitializer* init) {}

#undef DEF_TRAVERSE_CUSTOM_TYPE_PTR
#undef DEF_TRAVERSE_CUSTOM_TYPE
#undef DEF_TRAVERSE_TYPE_PTR
#undef DEF_TRAVERSE_TYPE

protected:
	CxxAstVisitor* getAstVisitor();
	const CxxAstVisitor* getAstVisitor() const;

private:
	CxxAstVisitor* m_astVisitor;
};

#endif // CXX_AST_VISITOR_COMPONENT_H
