#ifndef QT_ICON_BUTTON_H
#define QT_ICON_BUTTON_H

#include <QPushButton>

class QtIconButton
	: public QPushButton
{
public:
	QtIconButton(QString iconPath, QString hoveredIconPath, QWidget* parent = nullptr);

protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

private:
	QString m_iconPath;
	QString m_hoveredIconPath;
};

#endif // QT_ICON_BUTTON_H
