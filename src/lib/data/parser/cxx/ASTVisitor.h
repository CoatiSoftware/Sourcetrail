#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "data/parser/cxx/ASTBodyVisitorClient.h"
#include "data/parser/ParserClient.h"

class ASTVisitor
	: public clang::RecursiveASTVisitor<ASTVisitor>
	, public ASTBodyVisitorClient
{
public:
	ASTVisitor(clang::ASTContext* context, ParserClient* client);
	virtual ~ASTVisitor();

	// Left for debugging purposes. Uncomment to see a colored ast-dump of the parsed file.
	// virtual bool VisitTranslationUnitDecl(clang::TranslationUnitDecl* decl)
	// {
	// 	decl->dump();
	// 	return true;
	// }

	// RecursiveASTVisitor implementation
	virtual bool VisitStmt(const clang::Stmt* statement); // avoid visiting

	virtual bool VisitTypedefDecl(clang::TypedefDecl* declaration); // typedefs
	virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl* declaration); // structs, classes and inheritance
	virtual bool VisitVarDecl(clang::VarDecl* declaration); // global variables and static fields
	virtual bool VisitFieldDecl(clang::FieldDecl* declaration); // fields
	virtual bool VisitFunctionDecl(clang::FunctionDecl* declaration); // functions
	virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl* declaration); // methods
	virtual bool VisitCXXConstructorDecl(clang::CXXConstructorDecl* declaration); // initialization list
	virtual bool VisitNamespaceDecl(clang::NamespaceDecl* declaration); // namespaces
	virtual bool VisitEnumDecl(clang::EnumDecl* declaration); // enums
	virtual bool VisitEnumConstantDecl(clang::EnumConstantDecl* declaration); // enum fields

	virtual bool VisitTemplateTypeParmDecl(clang::TemplateTypeParmDecl *declaration);
	virtual bool VisitClassTemplateDecl(clang::ClassTemplateDecl* declaration);
	virtual bool VisitClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* declaration);
	virtual bool VisitFunctionTemplateDecl(clang::FunctionTemplateDecl *declaration);

	// ASTBodyVisitorClient implementation
	virtual void VisitCallExprInDeclBody(clang::FunctionDecl* decl, clang::CallExpr* expr); // calls
	virtual void VisitCallExprInDeclBody(clang::VarDecl* decl, clang::CallExpr* expr); // calls in initialization of global variables
	virtual void VisitCXXConstructExprInDeclBody(clang::FunctionDecl* decl, clang::CXXConstructExpr* expr); // constructor calls
	virtual void VisitCXXConstructExprInDeclBody(clang::VarDecl* decl, clang::CXXConstructExpr* expr); // constructor calls of global variables
	virtual void VisitCXXNewExprInDeclBody(clang::FunctionDecl* decl, clang::CXXNewExpr* expr); // type use of new operator
	virtual void VisitCXXNewExprInDeclBody(clang::VarDecl* decl, clang::CXXNewExpr* expr); // type use of new operator in global space
	virtual void VisitMemberExprInDeclBody(clang::FunctionDecl* decl, clang::MemberExpr* expr); // field usages
	virtual void VisitGlobalVariableExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr); // global variable usage
	virtual void VisitEnumExprInDeclBody(clang::FunctionDecl* decl, clang::DeclRefExpr* expr); // enum field usage
	virtual void VisitEnumExprInDeclBody(clang::VarDecl* decl, clang::DeclRefExpr* expr); // enum field usage in global variable
	virtual void VisitVarDeclInDeclBody(clang::FunctionDecl* decl, clang::VarDecl* varDecl); // type usages

private:
	bool hasValidLocation(const clang::Decl* declaration) const;
	ParserClient::AccessType convertAccessType(clang::AccessSpecifier) const;

	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;
	ParseLocation getParseLocationForNamedDecl(clang::NamedDecl* decl, const clang::SourceLocation& loc) const;
	ParseLocation getParseLocationForNamedDecl(clang::NamedDecl* decl) const;
	ParseLocation getParseLocationOfFunctionBody(clang::FunctionDecl* decl) const;
	ParseLocation getParseLocationOfRecordBody(clang::CXXRecordDecl* decl) const;

	ParseTypeUsage getParseTypeUsage(clang::TypeLoc typeLoc, const clang::QualType& type) const;
	ParseTypeUsage getParseTypeUsageOfReturnType(clang::FunctionDecl* declaration) const;
	std::vector<ParseTypeUsage> getParameters(clang::FunctionDecl* declaration) const;

	ParseVariable getParseVariable(clang::DeclaratorDecl* declaration) const;
	ParseFunction getParseFunction(clang::FunctionDecl* declaration) const;

	clang::ASTContext* m_context;
	ParserClient* m_client;
};

#endif // AST_VISITOR_H
