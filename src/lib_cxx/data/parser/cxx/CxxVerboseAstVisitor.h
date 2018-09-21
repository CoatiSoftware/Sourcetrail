
#ifndef CXX_VERBOSE_AST_VISITOR_H
#define CXX_VERBOSE_AST_VISITOR_H

#include <clang/AST/TypeLoc.h>

#include "CxxAstVisitor.h"

class CanonicalFilePathCache;
class ParserClient;

class CxxVerboseAstVisitor: public CxxAstVisitor
{
public:
	CxxVerboseAstVisitor(
		clang::ASTContext* context,
		clang::Preprocessor* preprocessor,
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache,
		std::shared_ptr<IndexerStateInfo> indexerStateInfo
	);

private:
	typedef CxxAstVisitor base;

	virtual bool TraverseDecl(clang::Decl* d);
	virtual bool TraverseStmt(clang::Stmt* stmt);
	virtual bool TraverseTypeLoc(clang::TypeLoc tl);

	std::string getIndentString() const;
	std::string obfuscateName(const std::string& name) const;

	std::string typeLocClassToString(clang::TypeLoc tl) const
	{
		switch(tl.getTypeLocClass())
		{
#define STRINGIFY(X) #X
#define ABSTRACT_TYPE(Class, Base)
#define TYPE(Class, Base)				\
		case clang::TypeLoc::Class:		\
			return STRINGIFY(Class);
#include <clang/AST/TypeNodes.def>
		case clang::TypeLoc::TypeLocClass::Qualified:
			return "Qualified";
		}
		return "";
	}

	std::wstring m_currentFilePath;
	unsigned int m_indentation;
};

#endif // CXX_VERBOSE_AST_VISITOR_H
