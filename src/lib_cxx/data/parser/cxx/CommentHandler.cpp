#include "data/parser/cxx/CommentHandler.h"

#include "data/parser/cxx/CanonicalFilePathCache.h"
#include "data/parser/cxx/utilityClang.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"

CommentHandler::CommentHandler(
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
)
	: m_client(client)
	, m_canonicalFilePathCache(canonicalFilePathCache)
{
}

CommentHandler::~CommentHandler()
{
}

bool CommentHandler::HandleComment(clang::Preprocessor& preprocessor, clang::SourceRange sourceRange)
{
	const clang::SourceManager& sourceManager = preprocessor.getSourceManager();

	const clang::FileID fileId = sourceManager.getFileID(sourceRange.getBegin());
	FilePath filePath = m_canonicalFilePathCache->getCanonicalFilePath(fileId, sourceManager);

	if (m_canonicalFilePathCache->isProjectFile(fileId, sourceManager))
	{
		const clang::PresumedLoc& presumedBegin = sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
		const clang::PresumedLoc& presumedEnd = sourceManager.getPresumedLoc(sourceRange.getEnd(), false);

		m_client->recordComment(ParseLocation(
			filePath,
			presumedBegin.getLine(),
			presumedBegin.getColumn(),
			presumedEnd.getLine(),
			presumedEnd.getColumn()
		));
	}

	return false;
}
