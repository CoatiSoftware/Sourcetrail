#ifndef AST_CONSUMER_H
#define AST_CONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"

class CanonicalFilePathCache;
class CxxAstVisitor;
class FileRegister;
class ParserClient;

class ASTConsumer
	: public clang::ASTConsumer
{
public:
	explicit ASTConsumer(
		clang::ASTContext* context,
		clang::Preprocessor* preprocessor,
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<FileRegister> fileRegister,
		std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
	);

	virtual ~ASTConsumer();

	virtual void HandleTranslationUnit(clang::ASTContext& context) override;

private:
	std::shared_ptr<CxxAstVisitor> m_visitor;
};

#endif // AST_CONSUMER_H
