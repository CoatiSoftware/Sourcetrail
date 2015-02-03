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

	virtual QSize sizeHint() const;

private slots:
	void refreshClicked();
	void autoRefreshClicked();

private:
	QPushButton* m_refreshButton;
	QPushButton* m_autoRefreshButton;
};

#endif // QT_REFRESH_BAR_H
