#ifndef COMMENT_HANDLER_H
#define COMMENT_HANDLER_H

#include "clang/Lex/Preprocessor.h"

class FileRegister;
class ParserClient;

class CommentHandler
	: public clang::CommentHandler
{
public:
	CommentHandler(std::shared_ptr<ParserClient> client, std::shared_ptr<FileRegister> fileRegister);
	virtual ~CommentHandler();

	virtual bool HandleComment(clang::Preprocessor& preprocessor, clang::SourceRange sourceRange);

private:
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
};

#endif // COMMENT_HANDLER_H
