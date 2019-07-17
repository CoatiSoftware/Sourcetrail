#ifndef CODE_SNIPPET_PARAMS_H
#define CODE_SNIPPET_PARAMS_H

#include <memory>

#include "TimeStamp.h"
#include "types.h"

class SourceLocationFile;

struct CodeSnippetParams
{
	CodeSnippetParams();

	// comparefunction for snippetsorting
	static bool sort(const CodeSnippetParams& a, const CodeSnippetParams& b);
	static bool sortById(const CodeSnippetParams& a, const CodeSnippetParams& b);

	size_t startLineNumber;
	size_t endLineNumber;

	std::wstring title;
	std::wstring footer;
	std::string code;

	Id titleId;
	Id footerId;
	TimeStamp modificationTime;

	std::shared_ptr<SourceLocationFile> locationFile;

	int refCount;

	bool isCollapsed;

	bool isDeclaration;
	bool isDefinition;

	bool insertSnippet;
	bool reduced;
};

#endif // CODE_SNIPPET_PARAMS_H
