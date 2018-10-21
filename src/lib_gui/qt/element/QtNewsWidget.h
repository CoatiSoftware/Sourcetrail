#ifndef QT_NEWS_WIDGET_H
#define QT_NEWS_WIDGET_H

#include <QWidget>

class QtTextEdit;
class QJsonObject;

class QtNewsWidget
	: public QWidget
{
	Q_OBJECT

public:
	QtNewsWidget(QWidget* parent = nullptr);
	virtual ~QtNewsWidget() = default;

public slots:
	void updateNews();

private:
	void resetFlags();

	void setNews(const QString& news);
	void setImportant(bool important);

	bool checkConditions(const QJsonObject& conditions) const;
	void processFlags(const QJsonObject& flags);

	QtTextEdit* m_text;
};

#endif // QT_NEWS_WIDGET_H
