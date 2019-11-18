#ifndef CODE_SNIPPET_PARAMS_H
#define CODE_SNIPPET_PARAMS_H

#include <memory>

#include "TimeStamp.h"
#include "types.h"

class SourceLocationFile;

struct CodeSnippetParams
{
	static CodeSnippetParams merge(const CodeSnippetParams& a, const CodeSnippetParams& b);

	size_t startLineNumber = 0;
	size_t endLineNumber = 0;

	std::wstring title;
	std::wstring footer;
	std::string code;

	Id titleId = 0;
	Id footerId = 0;

	std::shared_ptr<SourceLocationFile> locationFile;
	bool hasAllSourceLocations = false;
	bool isOverview = false;
};

struct CodeFileParams
{
	static bool sort(const CodeFileParams& a, const CodeFileParams& b);
	static bool sortById(const CodeFileParams& a, const CodeFileParams& b);

	std::shared_ptr<SourceLocationFile> locationFile;
	TimeStamp modificationTime;
	size_t referenceCount = 0;

	bool isMinimized = true;
	bool isDeclaration = false;
	bool isDefinition = false;

	std::vector<CodeSnippetParams> snippetParams;
	std::shared_ptr<CodeSnippetParams> fileParams;	  // TODO: replace with std::optional
};

#endif	  // CODE_SNIPPET_PARAMS_H
