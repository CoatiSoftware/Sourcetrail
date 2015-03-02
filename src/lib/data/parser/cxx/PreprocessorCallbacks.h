#ifndef PREPROCESSOR_CALLBACKS_H
#define PREPROCESSOR_CALLBACKS_H

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/PPCallbacks.h"

class FileManager;
class ParserClient;

struct ParseLocation;

class PreprocessorCallbacks
	: public clang::PPCallbacks
{
public:
	explicit PreprocessorCallbacks(clang::SourceManager& sourceManager, ParserClient* client, FileManager* fileManager);

	virtual void InclusionDirective(
		clang::SourceLocation hashLocation, const clang::Token& includeToken, llvm::StringRef fileName, bool isAngled,
		clang::CharSourceRange fileNameRange, const clang::FileEntry* fileEntry, llvm::StringRef searchPath,
		llvm::StringRef relativePath, const clang::Module* imported);

private:
	ParseLocation getParseLocation(const clang::SourceRange& sourceRange) const;

	const clang::SourceManager& m_sourceManager;
	ParserClient* m_client;
	FileManager* m_fileManager;
};

#endif // PREPROCESSOR_CALLBACKS_H
