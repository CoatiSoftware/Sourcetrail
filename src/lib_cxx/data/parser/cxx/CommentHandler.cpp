#include "data/parser/cxx/CommentHandler.h"

#include "data/parser/cxx/CanonicalFilePathCache.h"
#include "data/parser/cxx/utilityClang.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"
#include "utility/file/FileRegister.h"

CommentHandler::CommentHandler(
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
)
	: m_client(client)
	, m_fileRegister(fileRegister)
	, m_canonicalFilePathCache(canonicalFilePathCache)
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

	clang::FileID fileId = sourceManager.getFileID(sourceRange.getBegin());

	// find the location file
	if (fileId.isValid())
	{
		const clang::FileEntry* fileEntry = sourceManager.getFileEntryForID(fileId);
		if (fileEntry != nullptr && fileEntry->isValid())
		{
			FilePath filePath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);
			if (m_fileRegister->hasFilePath(filePath))
			{
				m_client->recordComment(ParseLocation(
					filePath,
					presumedBegin.getLine(),
					presumedBegin.getColumn(),
					presumedEnd.getLine(),
					presumedEnd.getColumn()
				));
			}
		}
	}

	return false;
}
