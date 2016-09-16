#ifndef COMMENT_HANDLER_H
#define COMMENT_HANDLER_H

#include "clang/Lex/Preprocessor.h"

class FileRegister;
class ParserClient;

class CommentHandler
	: public clang::CommentHandler
{
public:
	CommentHandler(ParserClient* client, FileRegister* fileRegister);
	virtual ~CommentHandler();

	virtual bool HandleComment(clang::Preprocessor& preprocessor, clang::SourceRange sourceRange);

private:
	ParserClient* m_client;
	FileRegister* m_fileRegister;
};

#endif // COMMENT_HANDLER_H
