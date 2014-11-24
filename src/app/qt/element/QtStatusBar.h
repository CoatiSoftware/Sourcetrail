#ifndef QT_STATUS_BAR_H
#define QT_STATUS_BAR_H

#include <string>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>

class QtStatusBar : public QStatusBar
{
public:
	QtStatusBar(void);
	~QtStatusBar(void);

	void setText(const std::string& text, bool isError = false);
private:
	QLabel m_text;
};

#endif // !QT_STATUS_BAR_H
