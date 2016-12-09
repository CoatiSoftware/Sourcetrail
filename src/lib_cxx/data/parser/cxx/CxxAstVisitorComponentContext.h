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

	virtual void beginTraverseDecl(clang::Decl* d);
	virtual void endTraverseDecl(clang::Decl* d);

	virtual void beginTraverseTypeLoc(clang::TypeLoc tl);
	virtual void endTraverseTypeLoc(clang::TypeLoc tl);

	virtual void beginTraverseLambdaExpr(clang::LambdaExpr* s);
	virtual void endTraverseLambdaExpr(clang::LambdaExpr* s);

	virtual void beginTraverseFunctionDecl(clang::FunctionDecl* d);
	virtual void endTraverseFunctionDecl(clang::FunctionDecl* d);

	virtual void beginTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d);
	virtual void endTraverseClassTemplateSpecializationDecl(clang::ClassTemplateSpecializationDecl *d);

	virtual void beginTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d);
	virtual void endTraverseClassTemplatePartialSpecializationDecl(clang::ClassTemplatePartialSpecializationDecl* d);

	virtual void beginTraverseDeclRefExpr(clang::DeclRefExpr* s);
	virtual void endTraverseDeclRefExpr(clang::DeclRefExpr* s);

	virtual void beginTraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc loc);
	virtual void endTraverseTemplateSpecializationTypeLoc(clang::TemplateSpecializationTypeLoc loc);

	virtual void beginTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e);
	virtual void endTraverseUnresolvedLookupExpr(clang::UnresolvedLookupExpr* e);

	virtual void beginTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);
	virtual void endTraverseTemplateArgumentLoc(const clang::TemplateArgumentLoc& loc);

private:
	std::vector<std::shared_ptr<CxxContext>> m_contextStack;
	std::vector<std::shared_ptr<CxxContext>> m_templateArgumentContext;
};

#endif // CXX_AST_VISITOR_COMPONENT_CONTEXT_H
