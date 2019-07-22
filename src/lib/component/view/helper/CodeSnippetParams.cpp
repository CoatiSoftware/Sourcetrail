#include "CodeSnippetParams.h"

#include "SourceLocationFile.h"

CodeSnippetParams::CodeSnippetParams()
	: startLineNumber(0)
	, endLineNumber(0)
	, titleId(0)
	, footerId(0)
	, refCount(0)
	, isCollapsed(false)
	, isDeclaration(false)
	, isDefinition(false)
	, insertSnippet(false)
	, reduced(false)
{
}

bool CodeSnippetParams::sort(const CodeSnippetParams& a, const CodeSnippetParams& b)
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

	// different files
	if (aFilePath != bFilePath)
	{
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

	return a.startLineNumber < b.startLineNumber;
}

bool CodeSnippetParams::sortById(const CodeSnippetParams& a, const CodeSnippetParams& b)
{
	return a.locationFile->getSourceLocations().begin()->get()->getLocationId() <
		b.locationFile->getSourceLocations().begin()->get()->getLocationId();
}
