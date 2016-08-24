#ifndef QT_REFRESH_BAR_H
#define QT_REFRESH_BAR_H

#include <QFrame>

class QPushButton;

class QtRefreshBar
	: public QFrame
{
	Q_OBJECT

public:
	QtRefreshBar();
	virtual ~QtRefreshBar();

	void refreshStyle();

private slots:
	void refreshClicked();

private:
	QPushButton* m_refreshButton;
};

#endif // QT_REFRESH_BAR_H
