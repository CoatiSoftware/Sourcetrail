#include "data/parser/cxx/PreprocessorCallbacks.h"

#include "clang/Driver/Util.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/MacroArgs.h"

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
	clang::SourceLocation location, FileChangeReason reason, clang::SrcMgr::CharacteristicKind, clang::FileID prevID)
{
	FilePath filePath;

	const clang::FileEntry *fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry)
	{
		filePath = FilePath(fileEntry->getName()).canonical();
	}

	if (!filePath.empty() && m_fileRegister->hasFilePath(filePath) && !m_fileRegister->fileIsParsed(filePath))
	{
		m_currentPath = filePath;

		if (reason == EnterFile && !m_fileRegister->includeFileIsParsed(filePath))
		{
			m_client->onFileParsed(m_fileRegister->getFileInfo(filePath));
			m_fileRegister->markIncludeFileParsing(filePath);
		}
	}
	else
	{
		m_currentPath = FilePath();
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
		FilePath baseFilePath = FilePath(baseFileEntry->getName()).canonical();
		if (!m_fileRegister->hasFilePath(baseFilePath) || m_fileRegister->fileIsParsed(baseFilePath))
		{
			return;
		}

		FilePath includedFilePath = FilePath(fileEntry->getName()).canonical();
		if (m_fileRegister->hasFilePath(includedFilePath))
		{
			m_client->onFileIncludeParsed(
				getParseLocation(fileNameRange.getAsRange()),
				m_fileRegister->getFileInfo(baseFilePath),
				m_fileRegister->getFileInfo(includedFilePath)
			);
		}
	}
}

void PreprocessorCallbacks::MacroDefined(const clang::Token& macroNameToken, const clang::MacroDirective* macroDirective)
{
	if (!m_currentPath.empty())
	{
		// ignore builtin macros
		if (m_sourceManager.getSpellingLoc(macroNameToken.getLocation()).printToString(m_sourceManager)[0] == '<')
		{
			return;
		}

		NameHierarchy nameHierarchy;
		nameHierarchy.push(std::make_shared<NameElement>(macroNameToken.getIdentifierInfo()->getName().str()));

		m_client->onMacroDefineParsed(
			getParseLocation(macroNameToken), nameHierarchy, getParseLocation(macroDirective->getMacroInfo()));
	}
}

void PreprocessorCallbacks::MacroUndefined(
	const clang::Token& macroNameToken, const clang::MacroDefinition& macroDefinition)
{
	onMacroUsage(macroNameToken);
}

void PreprocessorCallbacks::Defined(
		const clang::Token& macroNameToken, const clang::MacroDefinition& macroDefinition, clang::SourceRange range)
{
	onMacroUsage(macroNameToken);
}

void PreprocessorCallbacks::Ifdef(clang::SourceLocation location, const clang::Token& macroNameToken,
		const clang::MacroDefinition& macroDefinition)
{
	onMacroUsage(macroNameToken);
}
void PreprocessorCallbacks::Ifndef(clang::SourceLocation location, const clang::Token& macroNameToken,
		const clang::MacroDefinition& macroDefinition)
{
	onMacroUsage(macroNameToken);
}

void PreprocessorCallbacks::MacroExpands(
	const clang::Token& macroNameToken, const clang::MacroDefinition& macroDirective,
	clang::SourceRange range, const clang::MacroArgs* args
){
	onMacroUsage(macroNameToken);
}

void PreprocessorCallbacks::onMacroUsage(const clang::Token& macroNameToken)
{
	if (!m_currentPath.empty())
	{
		NameHierarchy nameHierarchy;
		nameHierarchy.push(std::make_shared<NameElement>(macroNameToken.getIdentifierInfo()->getName().str()));

		m_client->onMacroExpandParsed(getParseLocation(macroNameToken), nameHierarchy);
	}
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::Token& macroNameTok) const
{
	const clang::SourceLocation& location = m_sourceManager.getSpellingLoc(macroNameTok.getLocation());
	const clang::SourceLocation& endLocation = m_sourceManager.getSpellingLoc(macroNameTok.getEndLoc());

	return ParseLocation(
		m_sourceManager.getFilename(location),
		m_sourceManager.getSpellingLineNumber(location),
		m_sourceManager.getSpellingColumnNumber(location),
		m_sourceManager.getSpellingLineNumber(endLocation),
		m_sourceManager.getSpellingColumnNumber(endLocation) - 1
	);
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::MacroInfo* macroInfo) const
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
