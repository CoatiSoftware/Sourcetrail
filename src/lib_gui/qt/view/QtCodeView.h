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
	virtual void clear();

	virtual void setActiveTokenIds(const std::vector<Id>& activeTokenIds);
	virtual void setErrorInfos(const std::vector<ErrorInfo>& errorInfos);

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds);
	virtual void addCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert);
	virtual void showCodeFile(const CodeSnippetParams& params);

	virtual void setFileState(const FilePath filePath, FileState state);

	virtual void showFirstActiveSnippet(const std::vector<Id>& activeTokenIds, bool scrollTo);
	virtual void showActiveTokenIds(const std::vector<Id>& activeTokenIds);
	virtual void showActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds);

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	virtual void defocusTokenIds();

	virtual void showContents();

	virtual void scrollToValue(int value);
	virtual void scrollToLine(std::string filename, unsigned int line);

private:
	void doRefreshView();
	void doClear();

	void doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds);
	void doAddCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert);
	void doShowCodeFile(const CodeSnippetParams& params);

	void doSetFileState(const FilePath filePath, FileState state);

	void doShowFirstActiveSnippet(const std::vector<Id>& activeTokenIds, bool scrollTo);
	void doShowActiveTokenIds(const std::vector<Id>& activeTokenIds);
	void doShowActiveLocalSymbolIds(const std::vector<Id>& localSymbolIds);

	void doFocusTokenIds(const std::vector<Id>& focusedTokenIds);
	void doDefocusTokenIds();

	void doShowContents();

	void doScrollToValue(int value);
	void doScrollToLine(std::string filename, unsigned int line);

	void setStyleSheet() const;

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<> m_clearFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&, const std::vector<Id>&> m_showCodeSnippetsFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&, bool> m_addCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_showCodeFileFunctor;
	QtThreadedFunctor<const FilePath, FileState> m_setFileStateFunctor;
	QtThreadedFunctor<const std::vector<Id>&, bool> m_doShowFirstActiveSnippetFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_doShowActiveTokenIdsFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_doShowActiveLocalSymbolIdsFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_focusTokenIdsFunctor;
	QtThreadedFunctor<> m_defocusTokenIdsFunctor;
	QtThreadedFunctor<> m_showContentsFunctor;
	QtThreadedFunctor<int> m_scrollToValueFunctor;
	QtThreadedFunctor<std::string, unsigned int> m_scrollToLineFunctor;

	QtCodeFileList* m_widget;

	std::vector<Id> m_activeTokenIds;
	std::vector<ErrorInfo> m_errorInfos;
};

# endif // QT_CODE_VIEW_H
