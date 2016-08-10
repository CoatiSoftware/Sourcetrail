#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>

#include "utility/types.h"

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QFrame;
class QtCodeNavigator;
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
	virtual void clear();

	virtual void setErrorInfos(const std::vector<ErrorInfo>& errorInfos);

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds);
	virtual void addCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert);

	virtual void setFileState(const FilePath filePath, FileState state);

	virtual void showActiveSnippet(
		const std::vector<Id>& activeTokenIds, std::shared_ptr<TokenLocationCollection> collection, bool scrollTo);
	virtual void showActiveTokenIds(const std::vector<Id>& activeTokenIds);
	virtual void showActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds);

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	virtual void defocusTokenIds();

	virtual void showContents();

	virtual void scrollToValue(int value);
	virtual void scrollToLine(const FilePath filePath, unsigned int line);

private:
	void doRefreshView();
	void doClear();

	void doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds);
	void doAddCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert);

	void doSetFileState(const FilePath filePath, FileState state);

	void doShowActiveSnippet(
		const std::vector<Id>& activeTokenIds, std::shared_ptr<TokenLocationCollection> collection, bool scrollTo);
	void doShowActiveTokenIds(const std::vector<Id>& activeTokenIds);
	void doShowActiveLocalSymbolIds(const std::vector<Id>& localSymbolIds);

	void doFocusTokenIds(const std::vector<Id>& focusedTokenIds);
	void doDefocusTokenIds();

	void doShowContents();

	void doScrollToValue(int value);
	void doScrollToLine(const FilePath filePath, unsigned int line);

	void setStyleSheet() const;

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<> m_clearFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&, const std::vector<Id>&> m_showCodeSnippetsFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&, bool> m_addCodeSnippetsFunctor;
	QtThreadedFunctor<const FilePath, FileState> m_setFileStateFunctor;
	QtThreadedFunctor<const std::vector<Id>&, std::shared_ptr<TokenLocationCollection>, bool> m_doShowActiveSnippetFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_doShowActiveTokenIdsFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_doShowActiveLocalSymbolIdsFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_focusTokenIdsFunctor;
	QtThreadedFunctor<> m_defocusTokenIdsFunctor;
	QtThreadedFunctor<> m_showContentsFunctor;
	QtThreadedFunctor<int> m_scrollToValueFunctor;
	QtThreadedFunctor<const FilePath, unsigned int> m_scrollToLineFunctor;

	QtCodeNavigator* m_widget;

	std::vector<ErrorInfo> m_errorInfos;
};

# endif // QT_CODE_VIEW_H
