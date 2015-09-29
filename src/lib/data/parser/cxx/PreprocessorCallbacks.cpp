#include "data/parser/cxx/PreprocessorCallbacks.h"


#include "clang/Driver/Util.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/MacroArgs.h"

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

	if (!fileEntry)
	{
		return;
	}

	FilePath filePath(fileEntry->getName());

	if (m_fileRegister->getFileManager()->hasFilePath(filePath.str()))
	{
		m_client->onFileParsed(m_fileRegister->getFileManager()->getFileInfo(filePath));
		m_fileRegister->markIncludeFileParsing(filePath.str());
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
		std::string includedFilePath = fileEntry->getName();

		const FileManager* fileManager = m_fileRegister->getFileManager();
		if (fileManager->hasFilePath(baseFilePath) && fileManager->hasFilePath(includedFilePath) &&
			!m_fileRegister->includeFileIsParsed(baseFilePath))
		{
			m_client->onFileIncludeParsed(
				getParseLocation(fileNameRange.getAsRange()),
				fileManager->getFileInfo(baseFilePath),
				fileManager->getFileInfo(includedFilePath)
			);
		}
	}
}

void PreprocessorCallbacks::MacroDefined(const clang::Token& macroNameToken, const clang::MacroDirective* macroDirective)
{
	const std::string& fileStr = m_sourceManager.getFilename(macroNameToken.getLocation());
	if (!fileStr.size())
	{
		return;
	}

	FilePath filePath = FilePath(fileStr);
	if (m_fileRegister->getFileManager()->hasFilePath(filePath) && !m_fileRegister->includeFileIsParsed(filePath))
	{
		// ignore builtin macros
		if (m_sourceManager.getSpellingLoc(macroNameToken.getLocation()).printToString(m_sourceManager)[0] == '<')
		{
			return;
		}

		NameHierarchy nameHierarchy;
		nameHierarchy.push(std::make_shared<NameElement>(macroNameToken.getIdentifierInfo()->getName().str()));

		m_client->onMacroDefineParsed(getParseLocation(macroNameToken), nameHierarchy);
	}
}

void PreprocessorCallbacks::MacroExpands(
	const clang::Token& macroNameToken, const clang::MacroDefinition& macroDirective,
	clang::SourceRange range, const clang::MacroArgs* args
){
	const std::string& fileStr = m_sourceManager.getFilename(macroNameToken.getLocation());
	if (!fileStr.size())
	{
		return;
	}

	FilePath filePath = FilePath(fileStr);
	if (m_fileRegister->getFileManager()->hasFilePath(filePath) && !m_fileRegister->includeFileIsParsed(filePath))
	{
		NameHierarchy nameHierarchy;
		nameHierarchy.push(std::make_shared<NameElement>(macroNameToken.getIdentifierInfo()->getName().str()));

		m_client->onMacroExpandParsed(getParseLocation(macroNameToken), nameHierarchy);
	}
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::Token& macroNameTok) const
{
	clang::SourceLocation location = macroNameTok.getLocation();
	return ParseLocation(
		m_sourceManager.getFilename(location),
		m_sourceManager.getSpellingLineNumber(location),
		m_sourceManager.getSpellingColumnNumber(location),
		m_sourceManager.getSpellingLineNumber(macroNameTok.getEndLoc()),
		m_sourceManager.getSpellingColumnNumber(macroNameTok.getEndLoc()) - 1
	);
}

ParseLocation PreprocessorCallbacks::getParseLocation(clang::MacroInfo* macroInfo) const
{
	clang::SourceLocation location = macroInfo->getDefinitionLoc();
	clang::SourceLocation endLocation = macroInfo->getDefinitionEndLoc();

	return ParseLocation(
		m_sourceManager.getFilename(location),
		m_sourceManager.getSpellingLineNumber(location),
		m_sourceManager.getSpellingColumnNumber(location),
		m_sourceManager.getSpellingLineNumber(endLocation),
		m_sourceManager.getSpellingColumnNumber(endLocation) - 1
	);
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
