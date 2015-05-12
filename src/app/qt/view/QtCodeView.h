#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/types.h"

class QFrame;
class QtCodeFileList;
class QWidget;

class QtCodeView: public CodeView
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

	virtual void focusToken(const Id tokenId);
	virtual void defocusToken();

private:
	void doRefreshView();
	void doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	void doShowCodeFile(const CodeSnippetParams& params);
	void doFocusToken(const Id tokenId);
	void doDefocusToken();

	std::shared_ptr<QtCodeFileList> createQtCodeFileList() const;

	void setStyleSheet(QWidget* widget) const;
	void clearClosedWindows();

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&> m_showCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_showCodeFileFunctor;
	QtThreadedFunctor<const Id&> m_focusTokenFunctor;
	QtThreadedFunctor<> m_defocusTokenFunctor;

	QtCodeFileList* m_widget;
	std::vector<std::shared_ptr<QtCodeFileList>> m_windows;

	std::vector<Id> m_activeTokenIds;
	std::vector<std::string> m_errorMessages;
};

# endif // QT_CODE_VIEW_H
