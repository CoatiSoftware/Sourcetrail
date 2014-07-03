#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>

#include <QFont>

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/types.h"

class QtCodeSnippet;
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
	virtual void addCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber);
	virtual void clearCodeSnippets();

	void activateToken(Id tokenId) const;

private:
	struct Snippet
	{
		std::shared_ptr<QtCodeSnippet> textField;
		std::shared_ptr<QtHighlighter> highlighter;
	};
	std::vector<std::shared_ptr<Snippet>> m_snippets;

	void doAddCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber);
	void doClearCodeSnippets();

	QFont m_font;

	QtThreadedFunctor<void> m_clearCodeSnippetsFunctor;
	QtThreadedFunctor<const std::string&, const TokenLocationFile&, int> m_addCodeSnippetFunctor;
};

# endif // QT_CODE_VIEW_H
