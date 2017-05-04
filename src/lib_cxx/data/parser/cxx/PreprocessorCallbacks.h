#ifndef PREPROCESSOR_CALLBACKS_H
#define PREPROCESSOR_CALLBACKS_H

#include <memory>

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Token.h"

#include "data/parser/cxx/cxxCacheTypes.h"
#include "utility/file/FilePath.h"

class FileRegister;
class ParserClient;

struct ParseLocation;

class PreprocessorCallbacks
	: public clang::PPCallbacks
{
public:
	explicit PreprocessorCallbacks(
		clang::SourceManager& sourceManager,
		std::shared_ptr<ParserClient> client,
		std::shared_ptr<FileRegister> fileRegister,
		std::shared_ptr<FilePathCache> canonicalFilePathCache);

	virtual void FileChanged(
		clang::SourceLocation location, FileChangeReason reason, clang::SrcMgr::CharacteristicKind, clang::FileID);

	virtual void InclusionDirective(
		clang::SourceLocation hashLocation, const clang::Token& includeToken, llvm::StringRef fileName, bool isAngled,
		clang::CharSourceRange fileNameRange, const clang::FileEntry* fileEntry, llvm::StringRef searchPath,
		llvm::StringRef relativePath, const clang::Module* imported);

	virtual void MacroDefined(const clang::Token& macroNameToken, const clang::MacroDirective* macroDirective);
	virtual void MacroUndefined(const clang::Token& macroNameToken, const clang::MacroDefinition& macroDefinition);

	virtual void Defined(
		const clang::Token& macroNameToken, const clang::MacroDefinition& macroDefinition, clang::SourceRange range);
	virtual void Ifdef(clang::SourceLocation location, const clang::Token& macroNameToken,
		const clang::MacroDefinition& macroDefinition);
	virtual void Ifndef(clang::SourceLocation location, const clang::Token& macroNameToken,
		const clang::MacroDefinition& macroDefinition);

	virtual void MacroExpands(
		const clang::Token& macroNameToken, const clang::MacroDefinition& macroDirective,
		clang::SourceRange range, const clang::MacroArgs* args
	);

private:
	void onMacroUsage(const clang::Token& macroNameToken);

	ParseLocation getParseLocation(const clang::Token& macroNameToc) const;
	ParseLocation getParseLocation(const clang::MacroInfo* macroNameToc) const;
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	const clang::SourceManager& m_sourceManager;
	std::shared_ptr<ParserClient> m_client;
	std::shared_ptr<FileRegister> m_fileRegister;
	std::shared_ptr<FilePathCache> m_canonicalFilePathCache;

	FilePath m_currentPath;
};

#endif // PREPROCESSOR_CALLBACKS_H
