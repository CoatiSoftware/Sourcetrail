#include "data/parser/cxx/PreprocessorCallbacks.h"

#include "utility/file/FileManager.h"
#include "utility/file/FileRegister.h"

#include "data/parser/ParserClient.h"
#include "data/parser/ParseLocation.h"

PreprocessorCallbacks::PreprocessorCallbacks(
	clang::SourceManager& sourceManager, ParserClient* client, FileRegister* fileRegister
)
	: m_sourceManager(sourceManager)
	, m_client(client)
	, m_fileRegister(fileRegister)
{
}

void PreprocessorCallbacks::FileChanged(
	clang::SourceLocation location, FileChangeReason reason, clang::SrcMgr::CharacteristicKind, clang::FileID)
{
	if (reason != EnterFile)
	{
		return;
	}

	const clang::FileEntry *fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry && m_fileRegister->getFileManager()->hasFilePath(fileEntry->getName()))
	{
		m_fileRegister->markIncludeFileParsing(fileEntry->getName());
	}
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

		if (m_fileRegister->getFileManager()->hasFilePath(baseFilePath) &&
			m_fileRegister->getFileManager()->hasFilePath(filePath))
		{
			m_client->onFileIncludeParsed(getParseLocation(fileNameRange.getAsRange()), baseFilePath, filePath);
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
