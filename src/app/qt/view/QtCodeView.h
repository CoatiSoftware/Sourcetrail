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
	virtual void showCodeFile(const CodeSnippetParams& params);
	virtual void addCodeSnippet(const CodeSnippetParams& params);
	virtual void clearCodeSnippets();

private:
	void doRefreshView();
	void doShowCodeFile(const CodeSnippetParams& params);
	void doAddCodeSnippet(const CodeSnippetParams& params);
	void doClearCodeSnippets();

	std::shared_ptr<QtCodeFileList> createQtCodeFileList() const;

	void setStyleSheet(QWidget* widget) const;

	void clearClosedWindows();

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<> m_clearCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_showCodeFileFunctor;
	QtThreadedFunctor<const CodeSnippetParams&> m_addCodeSnippetFunctor;

	std::shared_ptr<QtCodeFileList> m_widget;
	std::vector<std::shared_ptr<QtCodeFileList>> m_windows;
};

# endif // QT_CODE_VIEW_H
