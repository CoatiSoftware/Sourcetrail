#include "data/parser/cxx/CommentHandler.h"

#include "data/parser/ParserClient.h"
#include "utility/file/FileManager.h"
#include "utility/file/FileRegister.h"

CommentHandler::CommentHandler(ParserClient* client, FileRegister* fileRegister)
	: m_client(client)
	, m_fileRegister(fileRegister)
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

	FilePath filePath = FilePath(presumedBegin.getFilename());
	if (m_fileRegister->getFileManager()->hasFilePath(filePath) && !m_fileRegister->fileIsParsed(filePath))
	{
		m_client->onCommentParsed(ParseLocation(
			presumedBegin.getFilename(),
			presumedBegin.getLine(),
			presumedBegin.getColumn(),
			presumedEnd.getLine(),
			presumedEnd.getColumn()
		));
	}

	return false;
}
