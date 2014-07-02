#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>

#include <QFont>

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtHighlighter;
class QTextEdit;

class QtCodeView: public CodeView
{
public:
	QtCodeView(ViewLayout* viewLayout);
	~QtCodeView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initGui();

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

	void doAddCodeSnippet(std::string str);
	void doClearCodeSnippets();

	QFont m_font;

	QtThreadedFunctor<void> m_clearCodeSnippetsFunctor;
	QtThreadedFunctor<std::string> m_addCodeSnippetFunctor;
};

# endif // QT_CODE_VIEW_H
