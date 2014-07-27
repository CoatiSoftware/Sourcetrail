#ifndef CODE_VIEW_H
#define CODE_VIEW_H

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

		int startLineNumber;
		std::string code;
		TokenLocationFile locationFile;
		std::vector<Id> activeTokenIds;
	};

	CodeView(ViewLayout* viewLayout);
	virtual ~CodeView();

	virtual std::string getName() const;

	virtual void showCodeFile(const CodeSnippetParams& params) = 0;
	virtual void addCodeSnippet(const CodeSnippetParams& params) = 0;
	virtual void clearCodeSnippets() = 0;

private:
	CodeController* getController();
};

#endif // CODE_VIEW_H
