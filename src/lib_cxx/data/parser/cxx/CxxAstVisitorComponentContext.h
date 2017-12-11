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
	virtual ~CxxAstVisitorComponentContext();

	const clang::NamedDecl* getTopmostContextDecl() const;
	NameHierarchy getContextName(const size_t skip = 0);
	NameHierarchy getContextName(const NameHierarchy& fallback, const size_t skip = 0);

	virtual void beginTraverseDecl(clang::Decl* d) override;
	virtual void endTraverseDecl(clang::Decl* d) override;

	virtual void beginTraverseTypeLoc(const clang::TypeLoc& tl) override;
	virtual void endTraverseTypeLoc(const clang::TypeLoc& tl) override;

	virtual void beginTraverseLambdaExpr(clang::LambdaExpr* s) override;
	virtual void endTraverseLambdaExpr(clang::LambdaExpr* s) override;

	virtual void beginTraverseFunctionDecl(clang::FunctionDecl* d) override;
	virtual void endTraverseFunctionDecl(clang::FunctionDecl* d) override;

	virtual void beginTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d) override;
	virtual void endTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d) override;

	virtual void beginTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d) override;
	virtual void endTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d) override;

	virtual void beginTraverseDeclRefExpr(clang::DeclRefExpr* s) override;
	virtual void endTraverseDeclRefExpr(clang::DeclRefExpr* s) override;

	virtual void beginTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc) override;
	virtual void endTraverseTemplateSpecializationTypeLoc(const clang::TemplateSpecializationTypeLoc& loc) override;

	virtual void beginTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) override;
	virtual void endTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e) override;

	virtual void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;
	virtual void endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc) override;

private:
	std::vector<std::shared_ptr<CxxContext>> m_contextStack;
	std::vector<std::shared_ptr<CxxContext>> m_templateArgumentContext;
};

#endif // CXX_AST_VISITOR_COMPONENT_CONTEXT_H
