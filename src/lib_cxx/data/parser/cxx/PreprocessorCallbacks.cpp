#include "data/parser/cxx/PreprocessorCallbacks.h"

#include "clang/Driver/Util.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/MacroArgs.h"

#include "data/parser/cxx/CanonicalFilePathCache.h"
#include "data/parser/cxx/utilityClang.h"
#include "data/parser/ParserClient.h"
#include "data/parser/ParseLocation.h"

#include "utility/file/FileSystem.h"
#include "utility/file/FileRegister.h"
#include "utility/utilityString.h"

PreprocessorCallbacks::PreprocessorCallbacks(
	clang::SourceManager& sourceManager,
	std::shared_ptr<ParserClient> client,
	std::shared_ptr<FileRegister> fileRegister,
	std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache
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

	const clang::FileEntry* fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry != nullptr && fileEntry->isValid())
	{
		m_currentPath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);

		if (!m_currentPath.empty())
		{
			bool hasFilePath = m_fileRegister->hasFilePath(m_currentPath);
			m_client->recordFile(FileSystem::getFileInfoForPath(m_currentPath), hasFilePath); // todo: fix for tests
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
		const FilePath includedFilePath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);
		const NameHierarchy referencedNameHierarchy(includedFilePath.wstr(), NAME_DELIMITER_FILE);
		const NameHierarchy contextNameHierarchy(m_currentPath.wstr(), NAME_DELIMITER_FILE);

		m_client->recordReference(
			REFERENCE_INCLUDE,
			referencedNameHierarchy,
			contextNameHierarchy,
			getParseLocation(fileNameRange.getAsRange())
		);
	}
}

void PreprocessorCallbacks::MacroDefined(const clang::Token& macroNameToken, const clang::MacroDirective* macroDirective)
{
	if (!m_currentPath.empty() && m_fileRegister->hasFilePath(m_currentPath))
	{
		// ignore builtin macros
		if (m_sourceManager.getSpellingLoc(macroNameToken.getLocation()).printToString(m_sourceManager)[0] == '<')
		{
			return;
		}

		const NameHierarchy nameHierarchy(utility::decodeFromUtf8(macroNameToken.getIdentifierInfo()->getName().str()), NAME_DELIMITER_CXX);

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
	if (!m_currentPath.empty() && m_fileRegister->hasFilePath(m_currentPath) && isLocatedInProjectFile(macroNameToken.getLocation()))
	{
		const ParseLocation loc = getParseLocation(macroNameToken);

		const NameHierarchy referencedNameHierarchy(utility::decodeFromUtf8(macroNameToken.getIdentifierInfo()->getName().str()), NAME_DELIMITER_CXX);
		const NameHierarchy contextNameHierarchy(loc.filePath.wstr(), NAME_DELIMITER_FILE);

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

	const clang::FileEntry* fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry != nullptr && fileEntry->isValid())
	{
		return ParseLocation(
			m_canonicalFilePathCache->getCanonicalFilePath(fileEntry),
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

	const clang::FileEntry* fileEntry = m_sourceManager.getFileEntryForID(m_sourceManager.getFileID(location));
	if (fileEntry != nullptr && fileEntry->isValid())
	{
		return ParseLocation(
			m_canonicalFilePathCache->getCanonicalFilePath(fileEntry),
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
		if (fileEntry != nullptr && fileEntry->isValid())
		{
			return ParseLocation(
				m_canonicalFilePathCache->getCanonicalFilePath(fileEntry),
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

	clang::FileID fileId;
	if (spellingLoc.isValid())
	{
		fileId = m_sourceManager.getFileID(spellingLoc);
	}

	if (fileId.isValid())
	{
		auto it = m_inProjectFileMap.find(fileId);
		if (it != m_inProjectFileMap.end())
		{
			return it->second;
		}

		const clang::FileEntry* fileEntry = m_sourceManager.getFileEntryForID(fileId);
		if (fileEntry != nullptr && fileEntry->isValid())
		{
			const FilePath filePath = m_canonicalFilePathCache->getCanonicalFilePath(fileEntry);
			bool ret = m_fileRegister->hasFilePath(filePath);
			m_inProjectFileMap[fileId] = ret;
			return ret;
		}
	}

	return false;
}
