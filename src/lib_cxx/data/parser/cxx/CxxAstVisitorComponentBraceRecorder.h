#ifndef CXX_AST_VISITOR_COMPONENT_BRACE_RECORDER_H
#define CXX_AST_VISITOR_COMPONENT_BRACE_RECORDER_H

#include "data/parser/cxx/CxxAstVisitorComponent.h"

// This CxxAstVisitorComponent is responsible for recording all matching braces ["{", "}"] throughout the visited AST.
class CxxAstVisitorComponentBraceRecorder: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentBraceRecorder(CxxAstVisitor* astVisitor, clang::ASTContext* astContext, std::shared_ptr<ParserClient> client);

	void visitTagDecl(clang::TagDecl* d) override;
	void visitNamespaceDecl(clang::NamespaceDecl* d) override;
	void visitCompoundStmt(clang::CompoundStmt* s) override;
	void visitInitListExpr(clang::InitListExpr* s) override;
	void visitMSAsmStmt(clang::MSAsmStmt* s) override;

private:
	ParseLocation getParseLocation(const clang::SourceLocation& loc) const;
	void recordBraces(const ParseLocation& lbraceLoc, const ParseLocation& rbraceLoc);
	clang::SourceLocation getFirstLBraceLocation(clang::SourceLocation searchStartLoc) const;
	clang::SourceLocation getLastRBraceLocation(clang::SourceLocation searchStartLoc, clang::SourceLocation searchEndLoc) const;

	clang::ASTContext* m_astContext;
	std::shared_ptr<ParserClient> m_client;
};

#endif // CXX_AST_VISITOR_COMPONENT_BRACE_RECORDER_H
