#ifndef QT_STATUS_BAR_H
#define QT_STATUS_BAR_H

#include <string>

#include <QPushButton>
#include <QLabel>
#include <QStatusBar>

#include "data/ErrorCountInfo.h"

class QtStatusBar
	: public QStatusBar
{
	Q_OBJECT

public:
	QtStatusBar(void);
	virtual ~QtStatusBar(void);

	void setText(const std::string& text, bool isError, bool showLoader);
	void setErrorCount(ErrorCountInfo errorCount);

	void setIdeStatus(const std::string& text);

protected:
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void showStatus();
	void showErrors();

private:
	std::string m_textString;

	QPushButton m_text;
	QLabel m_loader;
	QPushButton m_errorButton;

	QLabel m_ideStatusText;
};

#endif // QT_STATUS_BAR_H
