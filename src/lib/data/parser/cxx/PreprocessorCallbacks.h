#ifndef PREPROCESSOR_CALLBACKS_H
#define PREPROCESSOR_CALLBACKS_H

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/PPCallbacks.h"

class FileRegister;
class ParserClient;

struct ParseLocation;

class PreprocessorCallbacks
	: public clang::PPCallbacks
{
public:
	explicit PreprocessorCallbacks(clang::SourceManager& sourceManager, ParserClient* client, FileRegister* fileRegister);

	virtual void FileChanged(clang::SourceLocation location, FileChangeReason reason, clang::SrcMgr::CharacteristicKind, clang::FileID);

	virtual void InclusionDirective(
		clang::SourceLocation hashLocation, const clang::Token& includeToken, llvm::StringRef fileName, bool isAngled,
		clang::CharSourceRange fileNameRange, const clang::FileEntry* fileEntry, llvm::StringRef searchPath,
		llvm::StringRef relativePath, const clang::Module* imported);

private:
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	const clang::SourceManager& m_sourceManager;
	ParserClient* m_client;
	FileRegister* m_fileRegister;
};

#endif // PREPROCESSOR_CALLBACKS_H
