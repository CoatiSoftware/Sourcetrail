#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>

#include "utility/types.h"

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QFrame;
class QtCodeFileList;
class QWidget;

class QtCodeView
	: public CodeView
{
public:
	QtCodeView(ViewLayout* viewLayout);
	~QtCodeView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// CodeView implementation
	virtual void setActiveTokenIds(const std::vector<Id>& activeTokenIds);
	virtual void setErrorMessages(const std::vector<std::string>& errorMessages);

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	virtual void showCodeFile(const CodeSnippetParams& params);

	virtual void scrollToFirstActiveSnippet();

	virtual void focusToken(const Id tokenId);
	virtual void defocusToken();

private:
	void doRefreshView();

	void doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	void doShowCodeFile(const CodeSnippetParams& params);

	void doScrollToFirstActiveSnippet();

	void doFocusToken(const Id tokenId);
	void doDefocusToken();

	void setStyleSheet() const;

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&> m_showCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_showCodeFileFunctor;
	QtThreadedFunctor<> m_doScrollToFirstActiveSnippetFunctor;
	QtThreadedFunctor<const Id&> m_focusTokenFunctor;
	QtThreadedFunctor<> m_defocusTokenFunctor;

	QtCodeFileList* m_widget;

	std::vector<Id> m_activeTokenIds;
	std::vector<std::string> m_errorMessages;
};

# endif // QT_CODE_VIEW_H
