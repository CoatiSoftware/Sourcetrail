#include "PreprocessorCallbacks.h"

#include <clang/Driver/Util.h>
#include <clang/Basic/IdentifierTable.h>
#include <clang/Lex/MacroArgs.h>

#include "CanonicalFilePathCache.h"
#include "utilityClang.h"
#include "ParserClient.h"
#include "ParseLocation.h"

#include "utilityString.h"

PreprocessorCallbacks::PreprocessorCallbacks(
	clang::SourceManager& sourceManager,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
)
	: m_sourceManager(sourceManager)
	, m_client(client)
	, m_canonicalFilePathCache(canonicalFilePathCache)
{
}

void PreprocessorCallbacks::FileChanged(
	clang::SourceLocation location, FileChangeReason reason, clang::SrcMgr::CharacteristicKind, clang::FileID prevID)
{
	const clang::FileID fileId = m_sourceManager.getFileID(location);
	const FilePath currentPath = m_canonicalFilePathCache->getCanonicalFilePath(fileId, m_sourceManager);
	m_currentPathIsProjectFile = false;

	if (!currentPath.empty())
	{
		m_currentPathIsProjectFile = m_canonicalFilePathCache->isProjectFile(fileId, m_sourceManager);

		if (m_fileWasRecorded.find(fileId) == m_fileWasRecorded.end())
		{
			m_currentFileSymbolId = m_client->recordFile(currentPath, m_currentPathIsProjectFile); // todo: fix for tests

			m_canonicalFilePathCache->addFileSymbolId(fileId, currentPath, m_currentFileSymbolId);
			m_fileWasRecorded.insert(fileId);
		}
		else
		{
			m_currentFileSymbolId = m_canonicalFilePathCache->getFileSymbolId(fileId);
		}
	}
}

void PreprocessorCallbacks::InclusionDirective(
	clang::SourceLocation hashLocation, const clang::Token& includeToken, llvm::StringRef fileName, bool isAngled,
	clang::CharSourceRange fileNameRange, const clang::FileEntry* fileEntry, llvm::StringRef searchPath,
	llvm::StringRef relativePath, const clang::Module* imported, clang::SrcMgr::CharacteristicKind fileType
){
	if (m_currentFileSymbolId && fileEntry)
	{
		const FilePath includedFilePath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);
		const NameHierarchy includedFileNameHierarchy(includedFilePath.wstr(), NAME_DELIMITER_FILE);

		Id includedFileSymbolId = m_client->recordSymbol(includedFileNameHierarchy);

		m_client->recordReference(
			REFERENCE_INCLUDE,
			includedFileSymbolId,
			m_currentFileSymbolId,
			getParseLocation(fileNameRange.getAsRange())
		);
	}
}

void PreprocessorCallbacks::MacroDefined(const clang::Token& macroNameToken, const clang::MacroDirective* macroDirective)
{
	if (m_currentPathIsProjectFile)
	{
		// ignore builtin macros
		if (m_sourceManager.getSpellingLoc(macroNameToken.getLocation()).printToString(m_sourceManager)[0] == '<')
		{
			return;
		}

		const NameHierarchy nameHierarchy(
			utility::decodeFromUtf8(macroNameToken.getIdentifierInfo()->getName().str()), NAME_DELIMITER_CXX);

		Id symbolId = m_client->recordSymbol(nameHierarchy);
		m_client->recordSymbolKind(symbolId, SYMBOL_MACRO);
		m_client->recordDefinitionKind(symbolId, DEFINITION_EXPLICIT);
		m_client->recordLocation(symbolId, getParseLocation(macroNameToken), ParseLocationType::TOKEN);
		m_client->recordLocation(symbolId, getParseLocation(macroDirective->getMacroInfo()), ParseLocationType::SCOPE);
	}
}

void PreprocessorCallbacks::MacroUndefined(
	const clang::Token& macroNameToken,
	const clang::MacroDefinition& macroDefinition,
	const clang::MacroDirective* macroUndefinition)
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
	if (m_currentPathIsProjectFile && isLocatedInProjectFile(macroNameToken.getLocation()))
	{
		const ParseLocation loc = getParseLocation(macroNameToken);

		const NameHierarchy referencedNameHierarchy(
			utility::decodeFromUtf8(macroNameToken.getIdentifierInfo()->getName().str()), NAME_DELIMITER_CXX);

		m_client->recordReference(
			REFERENCE_MACRO_USAGE,
			m_client->recordSymbol(referencedNameHierarchy),
			loc.fileId,
			loc
		);
	}
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::Token& macroNameTok) const
{
	const clang::SourceLocation& location = m_sourceManager.getSpellingLoc(macroNameTok.getLocation());
	const clang::SourceLocation& endLocation = m_sourceManager.getSpellingLoc(macroNameTok.getEndLoc());

	Id fileSymbolId = m_canonicalFilePathCache->getFileSymbolId(m_sourceManager.getFileID(location));
	if (fileSymbolId)
	{
		return ParseLocation(
			fileSymbolId,
			m_sourceManager.getSpellingLineNumber(location),
			m_sourceManager.getSpellingColumnNumber(location),
			m_sourceManager.getSpellingLineNumber(endLocation),
			m_sourceManager.getSpellingColumnNumber(endLocation) - 1
		);
	}

	return ParseLocation();
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::MacroInfo* macroInfo) const
{
	clang::SourceLocation location = macroInfo->getDefinitionLoc();
	clang::SourceLocation endLocation = macroInfo->getDefinitionEndLoc();

	Id fileSymbolId = m_canonicalFilePathCache->getFileSymbolId(m_sourceManager.getFileID(location));
	if (fileSymbolId)
	{
		return ParseLocation(
			fileSymbolId,
			m_sourceManager.getSpellingLineNumber(location),
			m_sourceManager.getSpellingColumnNumber(location),
			m_sourceManager.getSpellingLineNumber(endLocation),
			m_sourceManager.getSpellingColumnNumber(endLocation) - 1
		);
	}

	return ParseLocation();
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::SourceRange& sourceRange) const
{
	if (sourceRange.isValid())
	{
		const clang::PresumedLoc& presumedBegin = m_sourceManager.getPresumedLoc(sourceRange.getBegin(), false);
		const clang::PresumedLoc& presumedEnd = m_sourceManager.getPresumedLoc(sourceRange.getEnd(), false);

		Id fileSymbolId = m_canonicalFilePathCache->getFileSymbolId(
			m_sourceManager.getFileID(sourceRange.getBegin()));

		if (fileSymbolId)
		{
			return ParseLocation(
				fileSymbolId,
				presumedBegin.getLine(),
				presumedBegin.getColumn(),
				presumedEnd.getLine(),
				presumedEnd.getColumn() - 1
			);
		}
	}
	return ParseLocation();
}

bool PreprocessorCallbacks::isLocatedInProjectFile(const clang::SourceLocation loc)
{
	// we need the spelling loc here, since this is the location where the macro comes from
	clang::SourceLocation spellingLoc = m_sourceManager.getSpellingLoc(loc);
	if (!spellingLoc.isValid())
	{
		return false;
	}

	return m_canonicalFilePathCache->isProjectFile(m_sourceManager.getFileID(spellingLoc), m_sourceManager);
}
