#ifndef QT_CODE_FILE_TITLE_BAR_H
#define QT_CODE_FILE_TITLE_BAR_H

#include "qt/element/QtCodeFileTitleButton.h"
#include "qt/element/QtIconButton.h"

class QLabel;
class QtCodeFileTitleButton;
class QtIconStateButton;

class QtCodeFileTitleBar
	: public QtHoverButton
{
	Q_OBJECT

signals:
	void minimize();
	void snippet();
	void maximize();

public:
	QtCodeFileTitleBar(QWidget* parent = nullptr, bool isHovering = false);

	QtCodeFileTitleButton* getTitleButton() const;

	void setRefString(const QString& refString);

	void setMinimized(bool hasSnippets);
	void setSnippets();
	void setMaximized(bool hasSnippets);

	void updateFromOther(const QtCodeFileTitleBar* other);

private slots:
	void clickedTitleBar();

	void enteredTitleBar(QPushButton* button);
	void leftTitleBar(QPushButton* button);

	void clickedMinimizeButton();
	void clickedSnippetButton();
	void clickedMaximizeButton();

private:
	QtCodeFileTitleButton* m_titleButton;
	QLabel* m_referenceCount;

	QtIconStateButton* m_minimizeButton;
	QtIconStateButton* m_snippetButton;
	QtIconStateButton* m_maximizeButton;
};

#endif // QT_CODE_FILE_TITLE_BAR_H
