#ifndef CODE_VIEW_H
#define CODE_VIEW_H

#include <memory>

#include "utility/file/FilePath.h"

#include "component/view/helper/CodeSnippetParams.h"
#include "component/view/View.h"

class CodeController;

class CodeView
	: public View
{
public:
	enum FileState
	{
		FILE_MINIMIZED,
		FILE_SNIPPETS,
		FILE_MAXIMIZED
	};

	CodeView(ViewLayout* viewLayout);
	virtual ~CodeView();

	virtual std::string getName() const;

	virtual void clear() = 0;

	virtual void setActiveTokenIds(const std::vector<Id>& activeTokenIds) = 0;
	virtual void setErrorMessages(const std::vector<std::string>& errorMessages) = 0;

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds) = 0;
	virtual void addCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert) = 0;
	virtual void showCodeFile(const CodeSnippetParams& params) = 0;

	virtual void setFileState(const FilePath filePath, FileState state) = 0;

	virtual void showFirstActiveSnippet(const std::vector<Id>& activeTokenIds, bool scrollTo) = 0;
	virtual void showActiveTokenIds(const std::vector<Id>& activeTokenIds) = 0;
	virtual void showActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds) = 0;

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds) = 0;
	virtual void defocusTokenIds() = 0;

	virtual void showContents() = 0;

	virtual void scrollToValue(int value) = 0;

private:
	CodeController* getController();
};

#endif // CODE_VIEW_H
