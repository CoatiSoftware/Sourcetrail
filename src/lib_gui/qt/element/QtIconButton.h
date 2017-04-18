#ifndef QT_ICON_BUTTON_H
#define QT_ICON_BUTTON_H

#include <QPushButton>

class QtIconButton
	: public QPushButton
{
public:
	QtIconButton(QString iconPath, QString hoveredIconPath, QWidget* parent = nullptr);

	void setColor(QColor color);

protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

private:
	void setIconFromPath(QString path);

	QString m_iconPath;
	QString m_hoveredIconPath;

	QColor m_color;
};

#endif // QT_ICON_BUTTON_H
