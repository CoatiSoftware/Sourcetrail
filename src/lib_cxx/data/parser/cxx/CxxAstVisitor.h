#ifndef CXX_AST_VISITOR_H
#define CXX_AST_VISITOR_H

#include <memory>

#include <clang/AST/RecursiveASTVisitor.h>

#include "CxxAstVisitorComponentBraceRecorder.h"
#include "CxxAstVisitorComponentContext.h"
#include "CxxAstVisitorComponentDeclRefKind.h"
#include "CxxAstVisitorComponentImplicitCode.h"
#include "CxxAstVisitorComponentIndexer.h"
#include "CxxAstVisitorComponentTypeRefKind.h"
#include "CxxContext.h"

class CanonicalFilePathCache;
class ParserClient;
class FilePath;

struct IndexerStateInfo;
struct ParseLocation;

// methods are called in this order:
//	TraverseDecl()
//	`-	TraverseFunctionDecl()
// 		|-	WalkUpFromFunctionDecl()
// 		|	|-	WalkUpFromNamedDecl()
// 		|	|	|-	WalkUpFromDecl()
//	 	|	|	|	`-	VisitDecl()
// 		|	|	`-	VisitNamedDecl()
// 		|	`-	VisitFunctionDecl()
// 		`-	TraverseChildNodes()

class CxxAstVisitor: public clang::RecursiveASTVisitor<CxxAstVisitor>
{
public:
	CxxAstVisitor(
		clang::ASTContext* astContext,
		clang::Preprocessor* preprocessor,
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::shared_ptr<IndexerStateInfo> indexerStateInfo);
	virtual ~CxxAstVisitor() = default;

	template <typename T>
	T* getComponent();

	CanonicalFilePathCache* getCanonicalFilePathCache() const;

	// Indexing entry point
	void indexDecl(clang::Decl* d);

	// Visitor options
	virtual bool shouldVisitTemplateInstantiations() const;
	virtual bool shouldVisitImplicitCode() const;

	bool checkIgnoresTypeLoc(const clang::TypeLoc& tl) const;

	// Traversal methods. These specify how to traverse the AST and record context info.
	virtual bool TraverseDecl(clang::Decl* d);
	virtual bool TraverseQualifiedTypeLoc(clang::QualifiedTypeLoc tl);
	virtual bool TraverseTypeLoc(clang::TypeLoc tl);
	virtual bool TraverseType(clang::QualType t);
	virtual bool TraverseStmt(clang::Stmt* stmt);

	virtual bool TraverseCXXRecordDecl(clang::CXXRecordDecl* d);
	bool traverseCXXBaseSpecifier(const clang::CXXBaseSpecifier& d);
	virtual bool TraverseCXXMethodDecl(clang::CXXMethodDecl* d);
	virtual bool TraverseTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d);
	virtual bool TraverseTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d);
	virtual bool TraverseNestedNameSpecifierLoc(clang::NestedNameSpecifierLoc loc);
	virtual bool TraverseConstructorInitializer(clang::CXXCtorInitializer* init);
	virtual bool TraverseCallExpr(clang::CallExpr* s);
	virtual bool TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* s);
	virtual bool TraverseCXXOperatorCallExpr(clang::CXXOperatorCallExpr* s);
	virtual bool TraverseCXXConstructExpr(clang::CXXConstructExpr* s);
	virtual bool TraverseCXXTemporaryObjectExpr(clang::CXXTemporaryObjectExpr* s);
	virtual bool TraverseLambdaExpr(clang::LambdaExpr* s);
	virtual bool TraverseFunctionDecl(clang::FunctionDecl* d);
	virtual bool TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d);
	virtual bool TraverseClassTemplatePartialSpecializationDecl(
		clang::ClassTemplatePartialSpecializationDecl* d);
	virtual bool TraverseDeclRefExpr(clang::DeclRefExpr* s);
	virtual bool TraverseCXXForRangeStmt(clang::CXXForRangeStmt* s);
	virtual bool TraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc loc);
	virtual bool TraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* s);
	virtual bool TraverseUnresolvedMemberExpr(clang::UnresolvedMemberExpr* S);
	virtual bool TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);
	virtual bool TraverseLambdaCapture(
		clang::LambdaExpr* lambdaExpr, const clang::LambdaCapture* capture, clang::Expr* Init);
	virtual bool TraverseBinComma(clang::BinaryOperator* s);

	virtual bool TraverseDeclarationNameInfo(clang::DeclarationNameInfo NameInfo);

#define OPERATOR(NAME)                                                                             \
	virtual bool TraverseBin##NAME##Assign(clang::CompoundAssignOperator* s)                       \
	{                                                                                              \
		return TraverseAssignCommon(s);                                                            \
	}
	OPERATOR(Mul)
	OPERATOR(Div)
	OPERATOR(Rem)
	OPERATOR(Add)
	OPERATOR(Sub)
	OPERATOR(Shl)
	OPERATOR(Shr)
	OPERATOR(And)
	OPERATOR(Or)
	OPERATOR(Xor)
#undef OPERATOR


	void traverseDeclContextHelper(clang::DeclContext* d);
	bool TraverseCallCommon(clang::CallExpr* s);
	bool TraverseAssignCommon(clang::BinaryOperator* s);

	// Visitor methods. These actually record stuff and store it in the database.
	virtual bool VisitCastExpr(clang::CastExpr* s);
	virtual bool VisitUnaryAddrOf(clang::UnaryOperator* s);
	virtual bool VisitUnaryDeref(clang::UnaryOperator* s);
	virtual bool VisitDeclStmt(clang::DeclStmt* s);
	virtual bool VisitReturnStmt(clang::ReturnStmt* s);
	virtual bool VisitCompoundStmt(clang::CompoundStmt* s);
	virtual bool VisitInitListExpr(clang::InitListExpr* s);


	virtual bool VisitTagDecl(clang::TagDecl* d);
	virtual bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d);
	virtual bool VisitFunctionDecl(clang::FunctionDecl* d);
	virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl* d);
	virtual bool VisitVarDecl(clang::VarDecl* d);
	virtual bool VisitVarTemplateSpecializationDecl(clang::VarTemplateSpecializationDecl* d);
	virtual bool VisitFieldDecl(clang::FieldDecl* d);
	virtual bool VisitTypedefDecl(clang::TypedefDecl* d);
	virtual bool VisitTypeAliasDecl(clang::TypeAliasDecl* d);
	virtual bool VisitNamespaceDecl(clang::NamespaceDecl* d);
	virtual bool VisitNamespaceAliasDecl(clang::NamespaceAliasDecl* d);
	virtual bool VisitEnumConstantDecl(clang::EnumConstantDecl* d);
	virtual bool VisitUsingDirectiveDecl(clang::UsingDirectiveDecl* d);
	virtual bool VisitUsingDecl(clang::UsingDecl* d);
	virtual bool VisitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d);
	virtual bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d);
	virtual bool VisitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d);
	virtual bool VisitTranslationUnitDecl(clang::TranslationUnitDecl* d);

	virtual bool VisitTypeLoc(clang::TypeLoc tl);

	virtual bool VisitDeclRefExpr(clang::DeclRefExpr* s);
	virtual bool VisitMemberExpr(clang::MemberExpr* s);
	virtual bool VisitCXXDependentScopeMemberExpr(clang::CXXDependentScopeMemberExpr* s);
	virtual bool VisitCXXConstructExpr(clang::CXXConstructExpr* s);
	virtual bool VisitCXXDeleteExpr(clang::CXXDeleteExpr* s);
	virtual bool VisitLambdaExpr(clang::LambdaExpr* s);
	virtual bool VisitMSAsmStmt(clang::MSAsmStmt* s);
	virtual bool VisitConstructorInitializer(clang::CXXCtorInitializer* init);

	ParseLocation getParseLocationOfTagDeclBody(clang::TagDecl* decl) const;
	ParseLocation getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const;
	ParseLocation getParseLocation(const clang::SourceLocation& loc) const;
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	bool shouldVisitStmt(const clang::Stmt* s) const;
	bool shouldVisitDecl(const clang::Decl* decl) const;
	bool shouldVisitReference(const clang::SourceLocation& referenceLocation) const;

	bool isLocatedInProjectFile(clang::SourceLocation loc) const;

protected:
	typedef clang::RecursiveASTVisitor<CxxAstVisitor> Base;

	clang::ASTContext* m_astContext;
	clang::Preprocessor* m_preprocessor;
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo;
	std::shared_ptr<CanonicalFilePathCache> m_canonicalFilePathCache;

	CxxAstVisitorComponentContext m_contextComponent;
	CxxAstVisitorComponentDeclRefKind m_declRefKindComponent;
	CxxAstVisitorComponentTypeRefKind m_typeRefKindComponent;
	CxxAstVisitorComponentImplicitCode m_implicitCodeComponent;
	CxxAstVisitorComponentIndexer m_indexerComponent;
	CxxAstVisitorComponentBraceRecorder m_braceRecorderComponent;
};

template <>
CxxAstVisitorComponentContext* CxxAstVisitor::getComponent();

template <>
CxxAstVisitorComponentTypeRefKind* CxxAstVisitor::getComponent();

template <>
CxxAstVisitorComponentDeclRefKind* CxxAstVisitor::getComponent();

template <>
CxxAstVisitorComponentIndexer* CxxAstVisitor::getComponent();

#endif	  // CXX_AST_VISITOR_H
