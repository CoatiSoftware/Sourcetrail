#ifndef CXX_AST_VISITOR_COMPONENT_CONTEXT_H
#define CXX_AST_VISITOR_COMPONENT_CONTEXT_H

#include "data/parser/cxx/CxxAstVisitorComponent.h"
#include "data/parser/cxx/CxxContext.h"

// This CxxAstVisitorComponent is responsible for recording and providing the decl/type that acts as the context of the currently traversed/visited node.
// Example: void foo() { bar(); }
// For this snippet the declaration of "foo" serves as the context of the call to "bar"
class CxxAstVisitorComponentContext: public CxxAstVisitorComponent
{
public:
	CxxAstVisitorComponentContext(CxxAstVisitor* astVisitor);

	const clang::NamedDecl* getTopmostContextDecl() const;
	NameHierarchy getContextName(const size_t skip = 0);
	NameHierarchy getContextName(const NameHierarchy& fallback, const size_t skip = 0);

	void beginTraverseDecl(clang::Decl* d) override;
	void endTraverseDecl(clang::Decl* d) override;

	void beginTraverseTypeLoc(const clang::TypeLoc& tl) override;
	void endTraverseTypeLoc(const clang::TypeLoc& tl) override;

	void beginTraverseLambdaExpr(clang::LambdaExpr* s) override;
	void endTraverseLambdaExpr(clang::LambdaExpr* s) override;

	void beginTraverseFunctionDecl(clang::FunctionDecl* d) override;
	void endTraverseFunctionDecl(clang::FunctionDecl* d) override;

	void beginTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d) override;
	void endTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d) override;

	void beginTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d) override;
	void endTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d) override;

	void beginTraverseDeclRefExpr(clang::DeclRefExpr* s) override;
	void endTraverseDeclRefExpr(clang::DeclRefExpr* s) override;

	void beginTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc) override;
	void endTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc) override;

	void beginTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) override;
	void endTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) override;

	void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;
	void endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;

private:
	std::vector<std::shared_ptr<CxxContext>> m_contextStack;
	std::vector<std::shared_ptr<CxxContext>> m_templateArgumentContext;
};

#endif // CXX_AST_VISITOR_COMPONENT_CONTEXT_H
