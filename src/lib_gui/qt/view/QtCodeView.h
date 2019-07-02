#ifndef QT_CODE_VIEW_H
#define QT_CODE_VIEW_H

#include "CodeView.h"
#include "QtThreadedFunctor.h"

class QtCodeNavigator;

class QtCodeView
	: public CodeView
{
public:
	QtCodeView(ViewLayout* viewLayout);
	~QtCodeView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// ScreenSearchResponder implementation
	bool isVisible() const override;
	void findMatches(ScreenSearchSender* sender, const std::wstring& query) override;
	void activateMatch(size_t matchIndex) override;
	void deactivateMatch(size_t matchIndex) override;
	void clearMatches() override;

	// CodeView implementation
	void clear() override;

	void showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const CodeParams params) override;
	void updateCodeSnippets(const std::vector<CodeSnippetParams>& snippets) override;
	void scrollTo(const ScrollParams params) override;

	bool showsErrors() const override;

	void setFileState(const FilePath filePath, FileState state) override;

	void showActiveSnippet(
		const std::vector<Id>& activeTokenIds, std::shared_ptr<SourceLocationCollection> collection, bool scrollTo) override;
	void showActiveTokenIds(const std::vector<Id>& activeTokenIds) override;
	void showActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds) override;

	void focusTokenIds(const std::vector<Id>& focusedTokenIds) override;
	void defocusTokenIds() override;

	void showContents() override;

	bool isInListMode() const override;
	void setMode(bool listMode) override;

	bool hasSingleFileCached(const FilePath& filePath) const override;

private:
	void performScroll();
	void setStyleSheet() const;

	QtThreadedLambdaFunctor m_onQtThread;

	QtCodeNavigator* m_widget;

	ScrollParams m_scrollParams;
};

# endif // QT_CODE_VIEW_H
