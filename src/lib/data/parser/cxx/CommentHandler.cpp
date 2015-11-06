#include "data/parser/cxx/CommentHandler.h"

#include "data/parser/ParserClient.h"
CommentHandler::CommentHandler(ParserClient* client)
	: m_client(client)
{
}

CommentHandler::~CommentHandler()
{
}

bool CommentHandler::HandleComment(clang::Preprocessor& preprocessor, clang::SourceRange sourceRange)
{
	clang::SourceManager& sourceManager = preprocessor.getSourceManager();
	const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
	const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd(), false);
	
	m_client->onCommentParsed(ParseLocation(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedEnd.getLine(),
		presumedEnd.getColumn()
	));

	return false;
}
