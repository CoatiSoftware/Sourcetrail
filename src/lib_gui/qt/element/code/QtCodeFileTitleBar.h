#ifndef QT_CODE_FILE_TITLE_BAR_H
#define QT_CODE_FILE_TITLE_BAR_H

#include "QtCodeFileTitleButton.h"
#include "QtHoverButton.h"

class QLabel;
class QtCodeFileTitleButton;
class QtIconStateButton;
class QtSelfRefreshIconButton;

class QtCodeFileTitleBar: public QtHoverButton
{
	Q_OBJECT

signals:
	void minimize();
	void snippet();
	void maximize();

public:
	QtCodeFileTitleBar(QWidget* parent = nullptr, bool isHovering = false, bool isSingle = false);

	QtCodeFileTitleButton* getTitleButton() const;

	void setIsComplete(bool isComplete);
	void setIsIndexed(bool isIndexed);
	void updateRefCount(int refCount, bool hasErrors, size_t fatalErrorCount);

	bool isCollapsed() const;

	void setIsFocused(bool focused);

	void setMinimized();
	void setSnippets();
	void setMaximized();

	void updateFromOther(const QtCodeFileTitleBar* other);

private slots:
	void clickedTitleBar();
	void clickedExpandButton();
	void clickedCollapseButton();
	void clickedSnippetButton();
	void clickedMaximizeButton();

private:
	QtCodeFileTitleButton* m_titleButton;
	QPushButton* m_showErrorsButton;
	QLabel* m_referenceCount;

	QtSelfRefreshIconButton* m_expandButton;
	QtSelfRefreshIconButton* m_collapseButton;

	QtIconStateButton* m_snippetButton;
	QtIconStateButton* m_maximizeButton;
};

#endif	  // QT_CODE_FILE_TITLE_BAR_H
