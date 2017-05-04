#ifndef COMMENT_HANDLER_H
#define COMMENT_HANDLER_H

#include "clang/Lex/Preprocessor.h"

#include "data/parser/cxx/cxxCacheTypes.h"

class FileRegister;
class ParserClient;

class CommentHandler
	: public clang::CommentHandler
{
public:
	CommentHandler(
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<FileRegister> fileRegister,
		std::shared_ptr<FilePathCache> canonicalFilePathCache
	);

	virtual ~CommentHandler();

	virtual bool HandleComment(clang::Preprocessor& preprocessor, clang::SourceRange sourceRange);

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
	std::shared_ptr<FilePathCache> m_canonicalFilePathCache;
};

#endif // COMMENT_HANDLER_H
