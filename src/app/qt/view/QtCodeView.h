#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include <memory>
#include <vector>

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/types.h"

class QFrame;
class QtCodeFile;

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
	virtual void addCodeSnippet(const CodeSnippetParams params);
	virtual void clearCodeSnippets();

	void activateToken(Id tokenId) const;

private:
	void doRefreshView();
	void doAddCodeSnippet(const CodeSnippetParams params);
	void doClearCodeSnippets();

	void setStyleSheet();

	std::shared_ptr<QFrame> m_frame;
	std::vector<std::shared_ptr<QtCodeFile> > m_files;

	QtThreadedFunctor<> m_refreshViewFunctor;
	QtThreadedFunctor<> m_clearCodeSnippetsFunctor;
	QtThreadedFunctor<const CodeSnippetParams> m_addCodeSnippetFunctor;
};

# endif // QT_CODE_VIEW_H
