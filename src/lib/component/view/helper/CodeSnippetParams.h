#ifndef CODE_SNIPPET_PARAMS_H
#define CODE_SNIPPET_PARAMS_H

#include <memory>

#include "utility/TimePoint.h"
#include "utility/types.h"

class SourceLocationFile;

struct CodeSnippetParams
{
	CodeSnippetParams();

	// comparefunction for snippetsorting
	static bool sort(const CodeSnippetParams& a, const CodeSnippetParams& b);

	uint startLineNumber;
	uint endLineNumber;

	std::string title;
	std::string footer;
	std::string code;

	Id titleId;
	Id footerId;
	TimePoint modificationTime;

	std::shared_ptr<SourceLocationFile> locationFile;

	int refCount;

	bool isCollapsed;

	bool isDeclaration;
	bool isDefinition;

	bool reduced;
};

#endif // CODE_SNIPPET_PARAMS_H
