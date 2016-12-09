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
	CxxAstVisitorComponentIndexer(CxxAstVisitor* astVisitor, clang::ASTContext* astContext, ParserClient* client, FileRegister* fileRegister);
	virtual ~CxxAstVisitorComponentIndexer();

	virtual void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);
	virtual void beginTraverseLambdaCapture(clang::LambdaExpr *lambdaExpr, const clang::LambdaCapture *capture);

	virtual void visitTagDecl(clang::TagDecl* d);
	virtual void visitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d);
	virtual void visitVarDecl(clang::VarDecl* d);
	virtual void visitFieldDecl(clang::FieldDecl* d);
	virtual void visitFunctionDecl(clang::FunctionDecl* d);
	virtual void visitCXXMethodDecl(clang::CXXMethodDecl* d);
	virtual void visitEnumConstantDecl(clang::EnumConstantDecl* d);
	virtual void visitNamespaceDecl(clang::NamespaceDecl* d);
	virtual void visitNamespaceAliasDecl(clang::NamespaceAliasDecl* d);
	virtual void visitTypedefDecl(clang::TypedefDecl* d);
	virtual void visitTypeAliasDecl(clang::TypeAliasDecl* d);
	virtual void visitUsingDirectiveDecl(clang::UsingDirectiveDecl* d);
	virtual void visitUsingDecl(clang::UsingDecl* d);
	virtual void visitNonTypeTemplateParmDecl(clang::NonTypeTemplateParmDecl* d);
	virtual void visitTemplateTypeParmDecl(clang::TemplateTypeParmDecl* d);
	virtual void visitTemplateTemplateParmDecl(clang::TemplateTemplateParmDecl* d);

	virtual void visitTypeLoc(clang::TypeLoc tl);

	virtual void visitDeclRefExpr(clang::DeclRefExpr* s);
	virtual void visitMemberExpr(clang::MemberExpr* s);
	virtual void visitCXXConstructExpr(clang::CXXConstructExpr* s);
	virtual void visitLambdaExpr(clang::LambdaExpr* s);

	virtual void visitConstructorInitializer(clang::CXXCtorInitializer* init);

private:
	struct FileIdHash
	{
		size_t operator()(clang::FileID fileID) const
		{
			return fileID.getHashValue();
		}
	};

	ParseLocation getParseLocationOfTagDeclBody(clang::TagDecl* decl) const;
	ParseLocation getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const;
	ParseLocation getParseLocation(const clang::SourceLocation& loc) const;
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	ReferenceKind consumeDeclRefContextKind();
	SymbolKind getSymbolKind(clang::VarDecl* d);

	bool shouldVisitDecl(const clang::Decl* decl);
	bool shouldVisitReference(const clang::SourceLocation& referenceLocation, const clang::Decl* contextDecl);
	bool isLocatedInUnparsedProjectFile(clang::SourceLocation loc);
	bool isLocatedInProjectFile(clang::SourceLocation loc);

	clang::ASTContext* m_astContext;
	ParserClient* m_client;
	FileRegister* m_fileRegister;

	std::unordered_map<const clang::FileID, bool, FileIdHash> m_inUnparsedProjectFileMap;
	std::unordered_map<const clang::FileID, bool, FileIdHash> m_inProjectFileMap;
};

#endif // CXX_AST_VISITOR_COMPONENT_INDEXER_H
