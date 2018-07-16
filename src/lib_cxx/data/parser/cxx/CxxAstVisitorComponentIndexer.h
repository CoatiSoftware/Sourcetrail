#ifndef CXX_AST_VISITOR_COMPONENT_INDEXER_H
#define CXX_AST_VISITOR_COMPONENT_INDEXER_H

#include <unordered_map>

#include "data/parser/cxx/CxxAstVisitor.h"
#include "data/parser/cxx/CxxAstVisitorComponent.h"
#include "data/parser/ReferenceKind.h"
#include "data/parser/SymbolKind.h"

// This CxxAstVisitorComponent is responsible for recording all symbols and relations throughout the visited AST.
class CxxAstVisitorComponentIndexer: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentIndexer(CxxAstVisitor* astVisitor, clang::ASTContext* astContext, std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister);

	void beginTraverseNestedNameSpecifierLoc(const clang::NestedNameSpecifierLoc& loc) override;
	void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;
	void beginTraverseLambdaCapture(clang::LambdaExpr *lambdaExpr, const clang::LambdaCapture *capture) override;

	void visitTagDecl(clang::TagDecl* d) override;
	void visitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d) override;
	void visitVarDecl(clang::VarDecl* d) override;
	void visitVarTemplateSpecializationDecl(clang::VarTemplateSpecializationDecl* d) override;
	void visitFieldDecl(clang::FieldDecl* d) override;
	void visitFunctionDecl(clang::FunctionDecl* d) override;
	void visitCXXMethodDecl(clang::CXXMethodDecl* d) override;
	void visitEnumConstantDecl(clang::EnumConstantDecl* d) override;
	void visitNamespaceDecl(clang::NamespaceDecl* d) override;
	void visitNamespaceAliasDecl(clang::NamespaceAliasDecl* d) override;
	void visitTypedefDecl(clang::TypedefDecl* d) override;
	void visitTypeAliasDecl(clang::TypeAliasDecl* d) override;
	void visitUsingDirectiveDecl(clang::UsingDirectiveDecl* d) override;
	void visitUsingDecl(clang::UsingDecl* d) override;
	void visitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d) override;
	void visitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d) override;
	void visitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d) override;

	void visitTypeLoc(clang::TypeLoc tl) override;

	void visitCompoundStmt(clang::CompoundStmt* s) override;
	void visitInitListExpr(clang::InitListExpr* s) override;
	void visitDeclRefExpr(clang::DeclRefExpr* s) override;
	void visitMemberExpr(clang::MemberExpr* s) override;
	void visitCXXConstructExpr(clang::CXXConstructExpr* s) override;
	void visitLambdaExpr(clang::LambdaExpr* s) override;
	void visitMSAsmStmt(clang::MSAsmStmt* s) override;

	void visitConstructorInitializer(clang::CXXCtorInitializer* init) override;

private:
	void recordTemplateMemberSpecialization(
		const clang::MemberSpecializationInfo* memberSpecializationInfo,
		const NameHierarchy& context,
		const ParseLocation& location,
		SymbolKind symbolKind
	);

	void recordBraces(const ParseLocation& lbraceLoc, const ParseLocation& rbraceLoc);

	ParseLocation getParseLocationOfTagDeclBody(clang::TagDecl* decl) const;
	ParseLocation getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const;
	ParseLocation getParseLocation(const clang::SourceLocation& loc) const;
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	ReferenceKind consumeDeclRefContextKind();

	bool shouldVisitStmt(const clang::Stmt* s) const;
	bool shouldVisitDecl(const clang::Decl* decl) const;
	bool shouldVisitReference(const clang::SourceLocation& referenceLocation, const clang::Decl* contextDecl) const;

	clang::ASTContext* m_astContext;
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
};

#endif // CXX_AST_VISITOR_COMPONENT_INDEXER_H
