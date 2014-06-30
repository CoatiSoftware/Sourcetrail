#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>
#include <iostream>

#include <QFont>

#include "component/view/CodeView.h"

class QtHighlighter;
class QTextEdit;
class ViewManager;

class QtCodeView: public CodeView
{
public:
	QtCodeView(std::shared_ptr<ViewManager> viewManager);
	~QtCodeView();

	// CodeView implementation
	virtual void addCodeSnippet(std::string str);
	virtual void clearCodeSnippets();

private:
	struct Snippet
	{
		std::shared_ptr<QTextEdit> textField;
		std::shared_ptr<QtHighlighter> highlighter;
	};
	std::vector<std::shared_ptr<Snippet>> m_snippets;

	QFont m_font;
};

# endif // QT_CODE_VIEW_H
