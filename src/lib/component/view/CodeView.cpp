#include "component/view/CodeView.h"

#include "component/controller/CodeController.h"
#include "utility/file/FileSystem.h"

CodeView::CodeSnippetParams::CodeSnippetParams()
	: startLineNumber(0)
	, endLineNumber(0)
	, titleId(0)
	, locationFile(std::make_shared<TokenLocationFile>(""))
	, isActive(false)
	, isDeclaration(false)
{
}

bool CodeView::CodeSnippetParams::sort(const CodeSnippetParams& a, const CodeSnippetParams& b)
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
			return aFilePath.withoutExtension() < bFilePath.withoutExtension();
		}
	}

	return a.startLineNumber < b.startLineNumber;
}

CodeView::CodeView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

CodeView::~CodeView()
{
}

std::string CodeView::getName() const
{
	return "Code";
}

CodeController* CodeView::getController()
{
	return View::getController<CodeController>();
}
