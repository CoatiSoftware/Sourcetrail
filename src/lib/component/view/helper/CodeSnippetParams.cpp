#include "CodeSnippetParams.h"

#include "SourceLocationFile.h"

CodeSnippetParams CodeSnippetParams::merge(const CodeSnippetParams& a, const CodeSnippetParams& b)
{
	const CodeSnippetParams* first = a.startLineNumber < b.startLineNumber ? &a : &b;
	const CodeSnippetParams* second = a.startLineNumber > b.startLineNumber ? &a : &b;

	SourceLocationFile* aFile = a.locationFile.get();
	SourceLocationFile* bFile = b.locationFile.get();

	std::shared_ptr<SourceLocationFile> locationFile = std::make_shared<SourceLocationFile>(
		aFile->getFilePath(),
		aFile->getLanguage(),
		aFile->isWhole(),
		aFile->isComplete(),
		aFile->isIndexed());

	aFile->forEachSourceLocation(
		[&locationFile](SourceLocation* loc) { locationFile->addSourceLocationCopy(loc); });

	bFile->forEachSourceLocation(
		[&locationFile](SourceLocation* loc) { locationFile->addSourceLocationCopy(loc); });

	std::string code = first->code;

	std::string secondCode = second->code;
	size_t secondCodeStartIndex = 0;
	for (size_t i = second->startLineNumber; i <= first->endLineNumber; i++)
	{
		secondCodeStartIndex = secondCode.find("\n", secondCodeStartIndex) + 1;
	}
	code += secondCode.substr(secondCodeStartIndex, secondCode.npos);

	CodeSnippetParams params;
	params.startLineNumber = first->startLineNumber;
	params.endLineNumber = second->endLineNumber;
	params.title = first->title;
	params.titleId = first->titleId;
	params.footer = second->footer;
	params.footerId = second->footerId;
	params.code = code;
	params.locationFile = locationFile;

	return params;
}

bool CodeFileParams::sort(const CodeFileParams& a, const CodeFileParams& b)
{
	// sort definitions
	if (a.isDefinition && !b.isDefinition)
	{
		return true;
	}
	else if (!a.isDefinition && b.isDefinition)
	{
		return false;
	}

	// sort declarations
	if (a.isDeclaration && !b.isDeclaration)
	{
		return true;
	}
	else if (!a.isDeclaration && b.isDeclaration)
	{
		return false;
	}

	// sort whole files
	if (a.locationFile->isWhole() && !b.locationFile->isWhole())
	{
		return true;
	}
	else if (!a.locationFile->isWhole() && b.locationFile->isWhole())
	{
		return false;
	}

	const FilePath& aFilePath = a.locationFile->getFilePath();
	const FilePath& bFilePath = b.locationFile->getFilePath();

	// first header
	if (aFilePath.withoutExtension() == bFilePath.withoutExtension())
	{
		return aFilePath.extension() > bFilePath.extension();
	}
	// alphabetical filepath without extension
	else
	{
		return aFilePath.withoutExtension() < bFilePath.withoutExtension();
	}
}

bool CodeFileParams::sortById(const CodeFileParams& a, const CodeFileParams& b)
{
	return a.locationFile->getSourceLocations().begin()->get()->getLocationId() <
		b.locationFile->getSourceLocations().begin()->get()->getLocationId();
}
