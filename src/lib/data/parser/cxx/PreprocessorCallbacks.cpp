#include "data/parser/cxx/PreprocessorCallbacks.h"

#include "utility/file/FileManager.h"
#include "data/parser/ParserClient.h"
#include "data/parser/ParseLocation.h"

PreprocessorCallbacks::PreprocessorCallbacks(
	clang::SourceManager& sourceManager, ParserClient* client, FileManager* fileManager
)
	: m_sourceManager(sourceManager)
	, m_client(client)
	, m_fileManager(fileManager)
{
}

void PreprocessorCallbacks::InclusionDirective(
	clang::SourceLocation hashLocation, const clang::Token& includeToken, llvm::StringRef fileName, bool isAngled,
	clang::CharSourceRange fileNameRange, const clang::FileEntry* fileEntry, llvm::StringRef searchPath,
	llvm::StringRef relativePath, const clang::Module* imported
){
	const clang::FileEntry* baseFileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(hashLocation));
	if (fileEntry && baseFileEntry)
	{
		std::string baseFilePath = baseFileEntry->getName();
		std::string filePath = fileEntry->getName();

		if (m_fileManager->hasFilePath(baseFilePath) && m_fileManager->hasFilePath(filePath))
		{
			m_client->onFileIncludeParsed(
				getParseLocation(fileNameRange.getAsRange()), baseFileEntry->getName(), fileEntry->getName());
		}
	}
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::SourceRange& sourceRange) const
{
	if (sourceRange.isInvalid())
	{
		return ParseLocation();
	}

	const clang::PresumedLoc& presumedBegin = m_sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
	const clang::PresumedLoc& presumedEnd = m_sourceManager.getPresumedLoc(sourceRange.getEnd(), false);

	return ParseLocation(
		presumedBegin.getFilename(),
		presumedBegin.getLine(),
		presumedBegin.getColumn(),
		presumedEnd.getLine(),
		presumedEnd.getColumn() - 1
	);
}
