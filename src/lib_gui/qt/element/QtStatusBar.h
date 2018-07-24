#ifndef QT_STATUS_BAR_H
#define QT_STATUS_BAR_H

#include <memory>
#include <string>

#include <QPushButton>
#include <QLabel>
#include <QStatusBar>

#include "data/ErrorCountInfo.h"

class QProgressBar;

class QtStatusBar
	: public QStatusBar
{
	Q_OBJECT

public:
	QtStatusBar();

	void setText(const std::wstring& text, bool isError, bool showLoader);
	void setErrorCount(ErrorCountInfo errorCount);

	void setIdeStatus(const std::wstring& text);

	void showIndexingProgress(size_t progressPercent);
	void hideIndexingProgress();

protected:
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void showStatus();
	void showErrors();
	void clickedIndexingProgress();

private:
	QWidget* addPermanentVLine();

	std::shared_ptr<QMovie> m_movie;

	std::wstring m_textString;

	QPushButton m_text;
	QLabel m_loader;
	QPushButton m_errorButton;

	QLabel m_ideStatusText;

	QPushButton* m_indexingStatus;
	QProgressBar* m_indexingProgress;

	QWidget* m_vlineError;
	QWidget* m_vlineIndexing;
};

#endif // QT_STATUS_BAR_H
