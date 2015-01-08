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
	virtual void clearCodeSnippets();
	virtual void setActiveTokenIds(const std::vector<Id>& activeTokenIds);
	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	virtual void showCodeFile(const CodeSnippetParams& params);

private:
	void doRefreshView();
	void doClearCodeSnippets();
	void doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets);
	void doShowCodeFile(const CodeSnippetParams& params);

	std::shared_ptr<QtCodeFileList> createQtCodeFileList() const;

	void setStyleSheet(QWidget* widget) const;
	void clearClosedWindows();

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<> m_clearCodeSnippetsFunctor;
	QtThreadedFunctor<const std::vector<CodeSnippetParams>&> m_showCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_showCodeFileFunctor;

	std::shared_ptr<QtCodeFileList> m_widget;
	std::vector<std::shared_ptr<QtCodeFileList>> m_windows;
	std::vector<Id> m_activeTokenIds;
};

# endif // QT_CODE_VIEW_H
