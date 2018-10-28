#ifndef QT_ICON_BUTTON_H
#define QT_ICON_BUTTON_H

#include <QPushButton>

#include "FilePath.h"
#include "MessageListener.h"
#include "MessageRefreshUI.h"

#include "QtThreadedFunctor.h"

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
	QtIconButton(const FilePath& iconPath, const FilePath& hoveredIconPath, QWidget* parent = nullptr);
	~QtIconButton() = default;

	void setColor(QColor color);

protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);

private:
	void setIconFromPath(const FilePath& path);

	const FilePath m_iconPath;
	const FilePath m_hoveredIconPath;

	QColor m_color;
};


class QtSelfRefreshIconButton
	: public QPushButton
	, public MessageListener<MessageRefreshUI>
{
public:
	QtSelfRefreshIconButton(
		const QString& text, const FilePath& iconPath, const std::string& buttonKey, QWidget* parent = nullptr);
	~QtSelfRefreshIconButton() = default;

	void setIconPath(const FilePath& iconPath);

protected:
	void handleMessage(MessageRefreshUI* message) override;

	virtual void refresh();

private:
	QtThreadedLambdaFunctor m_onQtThread;

	FilePath m_iconPath;
	const std::string m_buttonKey;
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
		FilePath iconPath;
		QColor color;
	};

	QtIconStateButton(QWidget* parent = nullptr);

	void addState(ButtonState buttonState, const FilePath& iconPath, QColor color = Qt::transparent);

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
