#ifndef COMMENT_HANDLER_H
#define COMMENT_HANDLER_H

#include "clang/Lex/Preprocessor.h"

class ParserClient;

class CommentHandler
	: public clang::CommentHandler
{
public:
	CommentHandler(ParserClient* client);
	virtual ~CommentHandler();

	virtual bool HandleComment(clang::Preprocessor& preprocessor, clang::SourceRange sourceRange);

private:
	ParserClient* m_client;
};

#endif // COMMENT_HANDLER_H
