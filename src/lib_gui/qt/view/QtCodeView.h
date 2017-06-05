#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include "component/view/CodeView.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtCodeNavigator;

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

	virtual void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const CodeParams params);
	virtual void scrollTo(const ScrollParams params);

	virtual bool showsErrors() const;

	virtual void setFileState(const FilePath filePath, FileState state);

	virtual void showActiveSnippet(
		const std::vector<Id>& activeTokenIds, std::shared_ptr<SourceLocationCollection> collection, bool scrollTo);
	virtual void showActiveTokenIds(const std::vector<Id>& activeTokenIds);
	virtual void showActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds);

	virtual void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	virtual void defocusTokenIds();

	virtual void showContents();

	virtual bool isInListMode() const;
	virtual bool hasSingleFileCached(const FilePath& filePath) const;

private:
	void performScroll();
	void setStyleSheet() const;

	QtThreadedLambdaFunctor m_onQtThread;

	QtCodeNavigator* m_widget;

	ScrollParams m_scrollParams;
};

# endif // QT_CODE_VIEW_H
