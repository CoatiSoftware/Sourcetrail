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

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds);
	virtual void addCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert);
	virtual void showCodeFile(const CodeSnippetParams& params);

	virtual void setFileState(const FilePath filePath, FileState state);

	virtual void showFirstActiveSnippet(const std::vector<Id>& activeTokenIds);
	virtual void showActiveTokenIds(const std::vector<Id>& activeTokenIds);

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	virtual void defocusTokenIds();

	virtual void showContents();

private:
	void doRefreshView();

	void doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds);
	void doAddCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert);
	void doShowCodeFile(const CodeSnippetParams& params);

	void doSetFileState(const FilePath filePath, FileState state);

	void doShowFirstActiveSnippet(const std::vector<Id>& activeTokenIds);
	void doShowActiveTokenIds(const std::vector<Id>& activeTokenIds);

	void doFocusTokenIds(const std::vector<Id>& focusedTokenIds);
	void doDefocusTokenIds();

	void doShowContents() const;

	void setStyleSheet() const;

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&, const std::vector<Id>&> m_showCodeSnippetsFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&, bool> m_addCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_showCodeFileFunctor;
	QtThreadedFunctor<const FilePath, FileState> m_setFileStateFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_doShowFirstActiveSnippetFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_doShowActiveTokenIdsFunctor;
	QtThreadedFunctor<const std::vector<Id>&> m_focusTokenIdsFunctor;
	QtThreadedFunctor<> m_defocusTokenIdsFunctor;
	QtThreadedFunctor<> m_showContentsFunctor;

	QtCodeFileList* m_widget;

	std::vector<Id> m_activeTokenIds;
	std::vector<std::string> m_errorMessages;

	bool m_isExpanding;
};

# endif // QT_CODE_VIEW_H
