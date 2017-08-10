#ifndef QT_ICON_BUTTON_H
#define QT_ICON_BUTTON_H

#include <QPushButton>

class QtHoverButton
	: public QPushButton
{
	Q_OBJECT

public:
	QtHoverButton(QWidget* parent = nullptr);

signals:
	void hoveredIn(QPushButton*);
	void hoveredOut(QPushButton*);

protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
};


class QtIconButton
	: public QPushButton
{
	Q_OBJECT
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


class QtIconStateButton
	: public QPushButton
{
	Q_OBJECT

public:
	enum ButtonState
	{
		STATE_DEFAULT,
		STATE_HOVERED,
		STATE_DISABLED
	};

	struct State
	{
		QString iconPath;
		QColor color;
	};

	QtIconStateButton(QWidget* parent = nullptr);

	void addState(ButtonState buttonState, QString iconPath, QColor color = Qt::transparent);

	void hoverIn();
	void hoverOut();

signals:
	void hoveredIn(QPushButton*);
	void hoveredOut(QPushButton*);

protected:
	void changeEvent(QEvent *event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

private:
	void setState(State state);

	std::map<ButtonState, State> m_states;
};

#endif // QT_ICON_BUTTON_H
