#ifndef QT_UPDATE_CHECKER_WIDGET_H
#define QT_UPDATE_CHECKER_WIDGET_H

#include <memory>

#include <QWidget>

class QPushButton;

class QtUpdateCheckerWidget: public QWidget
{
	Q_OBJECT

public:
	QtUpdateCheckerWidget(QWidget* parent = nullptr);
	virtual ~QtUpdateCheckerWidget();

signals:
	void updateReceived();

private:
	void checkUpdate(bool force);
	void setDownloadUrl(const QString& url);

	QPushButton* m_button;

	std::shared_ptr<bool> m_deleteCheck;
};

#endif	  // QT_UPDATE_CHECKER_WIDGET_H
