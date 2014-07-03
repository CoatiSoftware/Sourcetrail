#ifndef CODE_VIEW_H
#define CODE_VIEW_H

#include "component/view/View.h"

class CodeController;
class TokenLocationFile;

class CodeView: public View
{
public:
	CodeView(ViewLayout* viewLayout);
	virtual ~CodeView();

	virtual std::string getName() const;

	virtual void addCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber) = 0;
	virtual void clearCodeSnippets() = 0;

private:
	CodeController* getController();
};

#endif // CODE_VIEW_H
