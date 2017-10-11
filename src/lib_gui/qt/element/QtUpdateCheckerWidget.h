#ifndef QT_UPDATE_CHECKER_WIDGET_H
#define QT_UPDATE_CHECKER_WIDGET_H

#include <QWidget>

class QPushButton;

class QtUpdateCheckerWidget
	: public QWidget
{
public:
	QtUpdateCheckerWidget(QWidget* parent = nullptr);

private:
	void checkUpdate(bool force);
	void setDownloadUrl(QString url);

	QPushButton* m_button;
};

#endif // QT_UPDATE_CHECKER_WIDGET_H
