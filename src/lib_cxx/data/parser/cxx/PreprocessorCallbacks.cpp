#include "data/parser/cxx/PreprocessorCallbacks.h"

#include "clang/Driver/Util.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/MacroArgs.h"

#include "utility/file/FileSystem.h"
#include "utility/file/FileRegister.h"

#include "data/parser/cxx/utilityCxxAstVisitor.h"
#include "data/parser/ParserClient.h"
#include "data/parser/ParseLocation.h"

PreprocessorCallbacks::PreprocessorCallbacks(
	clang::SourceManager& sourceManager,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<FilePathCache> canonicalFilePathCache
)
	: m_sourceManager(sourceManager)
	, m_client(client)
	, m_fileRegister(fileRegister)
	, m_canonicalFilePathCache(canonicalFilePathCache)
{
}

void PreprocessorCallbacks::FileChanged(
	clang::SourceLocation location, FileChangeReason reason, clang::SrcMgr::CharacteristicKind, clang::FileID prevID)
{
	m_currentPath = FilePath();

	FilePath filePath;

	const clang::FileEntry *fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry)
	{
		filePath = m_canonicalFilePathCache->getValue(utility::getFileNameOfFileEntry(fileEntry));
	}

	if (!filePath.empty() && m_fileRegister->hasFilePath(filePath))
	{
		m_client->onFileParsed(FileSystem::getFileInfoForPath(filePath)); // todo: fix for tests

		if (!m_fileRegister->fileIsIndexed(filePath))
		{
			m_currentPath = filePath;

			if (reason == EnterFile)
			{
				m_fileRegister->markFileIndexing(filePath);
			}
		}
	}
}

void PreprocessorCallbacks::InclusionDirective(
	clang::SourceLocation hashLocation, const clang::Token& includeToken, llvm::StringRef fileName, bool isAngled,
	clang::CharSourceRange fileNameRange, const clang::FileEntry* fileEntry, llvm::StringRef searchPath,
	llvm::StringRef relativePath, const clang::Module* imported
){
	if (!m_currentPath.empty() && fileEntry)
	{
		FilePath includedFilePath = m_canonicalFilePathCache->getValue(utility::getFileNameOfFileEntry(fileEntry));
		if (m_fileRegister->hasFilePath(includedFilePath))
		{
			const NameHierarchy referencedNameHierarchy(includedFilePath.str(), NAME_DELIMITER_FILE);
			const NameHierarchy contextNameHierarchy(m_currentPath.str(), NAME_DELIMITER_FILE);

			m_client->recordReference(
				REFERENCE_INCLUDE,
				referencedNameHierarchy,
				contextNameHierarchy,
				getParseLocation(fileNameRange.getAsRange())
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

		const NameHierarchy nameHierarchy(macroNameToken.getIdentifierInfo()->getName().str(), NAME_DELIMITER_CXX);

		m_client->recordSymbol(
			nameHierarchy,
			SYMBOL_MACRO,
			getParseLocation(macroNameToken),
			getParseLocation(macroDirective->getMacroInfo()),
			ACCESS_NONE,
			DEFINITION_EXPLICIT
		);
	}
}

void PreprocessorCallbacks::MacroUndefined(
	const clang::Token& macroNameToken, const clang::MacroDefinition& macroDefinition, const clang::MacroDirective* macroUndefinition)
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
		const ParseLocation loc = getParseLocation(macroNameToken);

		const NameHierarchy referencedNameHierarchy(macroNameToken.getIdentifierInfo()->getName().str(), NAME_DELIMITER_CXX);
		const NameHierarchy contextNameHierarchy(loc.filePath.str(), NAME_DELIMITER_FILE);

		m_client->recordReference(
			REFERENCE_MACRO_USAGE,
			referencedNameHierarchy,
			contextNameHierarchy,
			loc
		);
	}
}

ParseLocation PreprocessorCallbacks::getParseLocation(const clang::Token& macroNameTok) const
{
	const clang::SourceLocation& location = m_sourceManager.getSpellingLoc(macroNameTok.getLocation());
	const clang::SourceLocation& endLocation = m_sourceManager.getSpellingLoc(macroNameTok.getEndLoc());

	const clang::FileEntry *fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry)
	{
		return ParseLocation(
			m_canonicalFilePathCache->getValue(utility::getFileNameOfFileEntry(fileEntry)),
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

	const clang::FileEntry *fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry)
	{
		return ParseLocation(
			m_canonicalFilePathCache->getValue(utility::getFileNameOfFileEntry(fileEntry)),
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

		const clang::FileEntry *fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(sourceRange.getBegin()));
		if (fileEntry)
		{
			return ParseLocation(
				m_canonicalFilePathCache->getValue(utility::getFileNameOfFileEntry(fileEntry)),
				presumedBegin.getLine(),
				presumedBegin.getColumn(),
				presumedEnd.getLine(),
				presumedEnd.getColumn() - 1
			);
		}
	}
	return ParseLocation();
}
