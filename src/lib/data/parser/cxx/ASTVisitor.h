#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include <memory>

#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"

#include "data/parser/ParserClient.h"

class ASTVisitor: public clang::RecursiveASTVisitor<ASTVisitor>
{
public:
	ASTVisitor(clang::ASTContext* context, std::shared_ptr<ParserClient> client);
	virtual ~ASTVisitor();

	// Left for debugging purposes. Uncomment to see a colored ast-dump of the parsed file.
	// virtual bool VisitDecl(clang::Decl* declaration)
	// {
	// 	// declaration->print(llvm::outs());
	// 	declaration->dump();
	// 	return false;
	// }

	virtual bool VisitTypedefDecl(const clang::TypedefDecl* declaration); // typedefs
	virtual bool VisitCXXRecordDecl(clang::CXXRecordDecl* declaration); // classes and structs
	virtual bool VisitVarDecl(clang::VarDecl* declaration); // global variables and static fields
	virtual bool VisitFieldDecl(clang::FieldDecl* declaration); // fields
	virtual bool VisitFunctionDecl(clang::FunctionDecl* declaration); // functions
	virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl* declaration); // methods
	virtual bool VisitNamespaceDecl(clang::NamespaceDecl* declaration); // namespaces
	virtual bool VisitEnumDecl(clang::EnumDecl* declaration); // enums
	virtual bool VisitEnumConstantDecl(clang::EnumConstantDecl* declaration); // enum fields

private:
	bool hasValidLocation(const clang::Decl* declaration) const;
	ParseLocation getParseLocation(const clang::Decl* declaration) const;
	ParseVariable getParseVariable(clang::ValueDecl* declaration) const;
	std::vector<ParseVariable> getParameters(clang::FunctionDecl* declaration) const;
	std::string getTypeName(const clang::QualType& type) const;
	ParserClient::AccessType convertAccessType(clang::AccessSpecifier) const;

	clang::ASTContext* m_context;
	std::shared_ptr<ParserClient> m_client;
};

#endif // AST_VISITOR_H
