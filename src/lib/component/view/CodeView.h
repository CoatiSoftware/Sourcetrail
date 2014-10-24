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

		uint startLineNumber;
		uint endLineNumber;
		uint lineCount;

		std::string code;

		TokenLocationFile locationFile;		

		bool isActive;
		bool isDeclaration;

		//comparefunctions for snippetsorting
		static bool sort(CodeSnippetParams a, CodeSnippetParams b);
	};

	CodeView(ViewLayout* viewLayout);
	virtual ~CodeView();

	virtual std::string getName() const;

	virtual void showCodeFile(const CodeSnippetParams& params) = 0;
	virtual void addCodeSnippet(const CodeSnippetParams& params) = 0;
	virtual void clearCodeSnippets() = 0;
	virtual void setActiveTokenIds(std::vector<Id> ids) = 0;

private:
	CodeController* getController();
};

#endif // CODE_VIEW_H
