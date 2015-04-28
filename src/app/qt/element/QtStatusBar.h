#ifndef QT_STATUS_BAR_H
#define QT_STATUS_BAR_H

#include <string>
#include <QStatusBar>
#include <QLabel>

class QtStatusBar
	: public QStatusBar
{
public:
	QtStatusBar(void);
	~QtStatusBar(void);

	void setText(const std::string& text, bool isError, bool showLoader);

private:
	QLabel m_text;
	QLabel m_loader;
};

#endif // QT_STATUS_BAR_H
