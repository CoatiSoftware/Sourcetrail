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
	virtual void addCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	virtual void showCodeFile(const CodeSnippetParams& params);

	virtual void showFirstActiveSnippet();

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	virtual void defocusTokenIds();

private:
	void doRefreshView();

	void doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	void doAddCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	void doShowCodeFile(const CodeSnippetParams& params);

	void doShowFirstActiveSnippet();

	void doFocusTokenIds(const std::vector<Id>& focusedTokenIds);
	void doDefocusTokenIds();

	void setStyleSheet() const;

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&> m_showCodeSnippetsFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&> m_addCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_showCodeFileFunctor;
	QtThreadedFunctor<> m_doShowFirstActiveSnippetFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_focusTokenIdsFunctor;
	QtThreadedFunctor<> m_defocusTokenIdsFunctor;

	QtCodeFileList* m_widget;

	std::vector<Id> m_activeTokenIds;
	std::vector<std::string> m_errorMessages;
};

# endif // QT_CODE_VIEW_H
