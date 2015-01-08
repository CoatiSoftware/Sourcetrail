#include "component/view/CodeView.h"

#include "component/controller/CodeController.h"
#include "utility/FileSystem.h"

CodeView::CodeSnippetParams::CodeSnippetParams()
	: startLineNumber(0)
	, endLineNumber(0)
	, lineCount(0)
	, locationFile("")
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

	// different files
	if (a.locationFile.getFilePath() != b.locationFile.getFilePath())
	{
		// first header
		if (FileSystem::filePathWithoutExtension(a.locationFile.getFilePath()) ==
			FileSystem::filePathWithoutExtension(b.locationFile.getFilePath()))
		{
			return FileSystem::extension(a.locationFile.getFilePath()) >
				FileSystem::extension(b.locationFile.getFilePath());
		}
		// alphabetical filepath without extension
		else
		{
			return FileSystem::filePathWithoutExtension(a.locationFile.getFilePath())
				< FileSystem::filePathWithoutExtension(b.locationFile.getFilePath());
		}
	}

	return a.startLineNumber < b.startLineNumber;
}

CodeView::CodeView(ViewLayout* viewLayout)
	: View(viewLayout, Vec2i(100, 100))
{
}

CodeView::~CodeView()
{
}

std::string CodeView::getName() const
{
	return "CodeView";
}

CodeController* CodeView::getController()
{
	return View::getController<CodeController>();
}
