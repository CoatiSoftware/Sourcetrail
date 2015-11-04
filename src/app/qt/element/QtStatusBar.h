#ifndef QT_STATUS_BAR_H
#define QT_STATUS_BAR_H

#include <string>

#include <QPushButton>
#include <QLabel>
#include <QStatusBar>

class QtStatusBar
	: public QStatusBar
{
	Q_OBJECT

public:
	QtStatusBar(void);
	virtual ~QtStatusBar(void);

	void setText(const std::string& text, bool isError, bool showLoader);
	void setErrorCount(size_t count);

private slots:
	void showErrors();

private:
	QLabel m_text;
	QLabel m_loader;
	QPushButton m_errorButton;
};

#endif // QT_STATUS_BAR_H
