#ifndef CXX_AST_VISITOR_H
#define CXX_AST_VISITOR_H

#include <unordered_map>
#include <vector>

#include "clang/AST/ASTContext.h"
#include <clang/AST/RecursiveASTVisitor.h>

#include "data/parser/cxx/CxxContext.h"
#include "data/parser/AccessKind.h"
#include "data/parser/ReferenceKind.h"
#include "data/parser/SymbolKind.h"
#include "utility/messaging/MessageInterruptTasksCounter.h"
#include "utility/Cache.h"

class ParserClient;
struct ParseLocation;
class FileRegister;

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

class ScopedContextKindSetter
{
public:
	ScopedContextKindSetter(const ReferenceKind refKind, std::vector<ReferenceKind>* stack)
		: m_stack(stack)
	{
		m_stack->push_back(refKind);
	}

	~ScopedContextKindSetter()
	{
		m_stack->pop_back();
	}
private:
	std::vector<ReferenceKind>* m_stack;
};

class CxxAstVisitor: public clang::RecursiveASTVisitor<CxxAstVisitor>
{
public:
	CxxAstVisitor(clang::ASTContext* astContext, clang::Preprocessor* preprocessor, ParserClient* client, FileRegister* fileRegister);
	virtual ~CxxAstVisitor();

	// Indexing entry point
    void indexDecl(clang::Decl *d);

	// Visitor options
	virtual bool shouldVisitTemplateInstantiations() const;
	virtual bool shouldVisitImplicitCode() const;

	// Traversal methods. These specify how to traverse the AST and record context info.
	virtual bool TraverseDecl(clang::Decl *d);
	virtual bool TraverseTypeLoc(clang::TypeLoc tl);
	virtual bool TraverseType(clang::QualType t);
	virtual bool TraverseStmt(clang::Stmt *stmt);

	virtual bool TraverseCXXRecordDecl(clang::CXXRecordDecl *d);
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
	virtual bool TraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d);
	virtual bool TraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d);
	virtual bool TraverseDeclRefExpr(clang::DeclRefExpr* s);
	virtual bool TraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc loc);
	virtual bool TraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* s);
	virtual bool TraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);
	void traverseDeclContextHelper(clang::DeclContext *d);
	bool TraverseCallCommon(clang::CallExpr* s);

	// Visitor methods. These actually record stuff and store it in the database.
	virtual bool VisitTagDecl(clang::TagDecl* d);
	virtual bool VisitClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl* d);
	virtual bool VisitFunctionDecl(clang::FunctionDecl* d);
	virtual bool VisitCXXMethodDecl(clang::CXXMethodDecl* d);
	virtual bool VisitVarDecl(clang::VarDecl* d);
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

	virtual bool VisitTypeLoc(clang::TypeLoc tl);

	virtual bool VisitDeclRefExpr(clang::DeclRefExpr* s);
	virtual bool VisitMemberExpr(clang::MemberExpr* s);
	virtual bool VisitCXXConstructExpr(clang::CXXConstructExpr* s);
	virtual bool VisitLambdaExpr(clang::LambdaExpr* s);
	virtual bool VisitConstructorInitializer(clang::CXXCtorInitializer* init);

protected:
	// General helpers
	bool isImplicit(const clang::Decl* d) const;
	bool shouldVisitDecl(const clang::Decl* decl);
	bool shouldVisitReference(const clang::SourceLocation& referenceLocation, const clang::Decl* contextDecl);
	bool isLocatedInUnparsedProjectFile(clang::SourceLocation loc);
	bool isLocatedInProjectFile(clang::SourceLocation loc);

	ParseLocation getParseLocationOfTagDeclBody(clang::TagDecl* decl) const;
	ParseLocation getParseLocationOfFunctionBody(const clang::FunctionDecl* decl) const;
	ParseLocation getParseLocation(const clang::SourceLocation& loc) const;
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;
	AccessKind convertAccessSpecifier(clang::AccessSpecifier access) const;
	SymbolKind convertTagKind(clang::TagTypeKind tagKind);

private:
	ReferenceKind consumeDeclRefContextKind();

	typedef clang::RecursiveASTVisitor<CxxAstVisitor> base;

	const clang::NamedDecl* getTopmostContextDecl() const;
	NameHierarchy getContextName(const size_t skip = 0) const;
	NameHierarchy getContextName(const NameHierarchy& fallback, const size_t skip = 0) const;
	bool checkIgnoresTypeLoc(const clang::TypeLoc& tl);

	struct FileIdHash
	{
		size_t operator()(clang::FileID fileID) const
		{
			return fileID.getHashValue();
		}
	};

	clang::ASTContext* m_astContext;
	clang::Preprocessor* m_preprocessor;
	ParserClient* m_client;
	FileRegister* m_fileRegister;

	MessageInterruptTasksCounter m_interruptCounter;

	ReferenceKind m_typeRefContext;
	ReferenceKind m_declRefContext;
	std::vector<std::shared_ptr<CxxContext>> m_contextStack;
	std::shared_ptr<CxxContext> m_templateArgumentContext;

	std::shared_ptr<DeclNameCache> m_declNameCache;
	std::shared_ptr<TypeNameCache> m_typeNameCache;
	std::unordered_map<const clang::FileID, bool, FileIdHash> m_inUnparsedProjectFileMap;
	std::unordered_map<const clang::FileID, bool, FileIdHash> m_inProjectFileMap;
};

#endif // CXX_AST_VISITOR_H
