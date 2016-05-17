#include "component/view/helper/CodeSnippetParams.h"

#include "data/location/TokenLocationFile.h"

CodeSnippetParams::CodeSnippetParams()
	: startLineNumber(0)
	, endLineNumber(0)
	, titleId(0)
	, footerId(0)
	, locationFile()
	, refCount(0)
	, isActive(false)
	, isCollapsed(false)
	, isDeclaration(false)
	, isDefinition(false)
	, reduced(false)
{
}

bool CodeSnippetParams::sort(const CodeSnippetParams& a, const CodeSnippetParams& b)
{
	// sort active snippet first
	if (a.isActive && !b.isActive)
	{
		return true;
	}
	else if (!a.isActive && b.isActive)
	{
		return false;
	}

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
	if (a.locationFile->isWholeCopy && !b.locationFile->isWholeCopy)
	{
		return true;
	}
	else if (!a.locationFile->isWholeCopy && b.locationFile->isWholeCopy)
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
			return aFilePath.withoutExtension().fileName() < bFilePath.withoutExtension().fileName();
		}
	}

	return a.startLineNumber < b.startLineNumber;
}
