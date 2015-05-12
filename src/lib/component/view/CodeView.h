#ifndef CODE_VIEW_H
#define CODE_VIEW_H

#include <memory>

#include "component/view/View.h"
#include "data/location/TokenLocationFile.h"
#include "utility/types.h"

class CodeController;

class CodeView: public View
{
public:
	struct CodeSnippetParams
	{
		CodeSnippetParams();

		// comparefunction for snippetsorting
		static bool sort(const CodeSnippetParams& a, const CodeSnippetParams& b);

		uint startLineNumber;
		uint endLineNumber;
		uint lineCount;

		std::string title;
		std::string code;

		std::shared_ptr<TokenLocationFile> locationFile;

		bool isActive;
		bool isDeclaration;
	};

	CodeView(ViewLayout* viewLayout);
	virtual ~CodeView();

	virtual std::string getName() const;

	virtual void setActiveTokenIds(const std::vector<Id>& activeTokenIds) = 0;
	virtual void setErrorMessages(const std::vector<std::string>& errorMessages) = 0;

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets) = 0;
	virtual void showCodeFile(const CodeSnippetParams& params) = 0;

	virtual void focusToken(const Id tokenId) = 0;
	virtual void defocusToken() = 0;

private:
	CodeController* getController();
};

#endif // CODE_VIEW_H
