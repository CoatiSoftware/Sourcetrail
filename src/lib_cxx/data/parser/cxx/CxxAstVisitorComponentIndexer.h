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
	virtual ~CxxAstVisitorComponentIndexer();

	virtual void beginTraverseNestedNameSpecifierLoc(const clang::NestedNameSpecifierLoc& loc) override;
	virtual void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;
	virtual void beginTraverseLambdaCapture(clang::LambdaExpr *lambdaExpr, const clang::LambdaCapture *capture) override;

	virtual void visitTagDecl(clang::TagDecl* d) override;
	virtual void visitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d) override;
	virtual void visitVarDecl(clang::VarDecl* d) override;
	virtual void visitVarTemplateSpecializationDecl(clang::VarTemplateSpecializationDecl* d) override;
	virtual void visitFieldDecl(clang::FieldDecl* d) override;
	virtual void visitFunctionDecl(clang::FunctionDecl* d) override;
	virtual void visitCXXMethodDecl(clang::CXXMethodDecl* d) override;
	virtual void visitEnumConstantDecl(clang::EnumConstantDecl* d) override;
	virtual void visitNamespaceDecl(clang::NamespaceDecl* d) override;
	virtual void visitNamespaceAliasDecl(clang::NamespaceAliasDecl* d) override;
	virtual void visitTypedefDecl(clang::TypedefDecl* d) override;
	virtual void visitTypeAliasDecl(clang::TypeAliasDecl* d) override;
	virtual void visitUsingDirectiveDecl(clang::UsingDirectiveDecl* d) override;
	virtual void visitUsingDecl(clang::UsingDecl* d) override;
	virtual void visitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d) override;
	virtual void visitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d) override;
	virtual void visitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d) override;

	virtual void visitTypeLoc(clang::TypeLoc tl) override;

	virtual void visitDeclRefExpr(clang::DeclRefExpr* s) override;
	virtual void visitMemberExpr(clang::MemberExpr* s) override;
	virtual void visitCXXConstructExpr(clang::CXXConstructExpr* s) override;
	virtual void visitLambdaExpr(clang::LambdaExpr* s) override;

	virtual void visitConstructorInitializer(clang::CXXCtorInitializer* init) override;

private:
	void recordTemplateMemberSpecialization(
		const clang::MemberSpecializationInfo* memberSpecializationInfo, 
		const NameHierarchy& context, 
		const ParseLocation& location, 
		SymbolKind symbolKind
	);

	ParseLocation getParseLocationOfTagDeclBody(clang::TagDecl* decl) const;
	ParseLocation getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const;
	ParseLocation getParseLocation(const clang::SourceLocation& loc) const;
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	ReferenceKind consumeDeclRefContextKind();

	bool shouldVisitDecl(const clang::Decl* decl);
	bool shouldVisitReference(const clang::SourceLocation& referenceLocation, const clang::Decl* contextDecl);

	clang::ASTContext* m_astContext;
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
};

#endif // CXX_AST_VISITOR_COMPONENT_INDEXER_H
