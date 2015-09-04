#include "qt/element/QtSettingsWindow.h"

#include <QGraphicsDropShadowEffect>
#include <QSysInfo>

#include "utility/messaging/type/MessageInterruptTasks.h"

QtSettingsWindow::QtSettingsWindow(QWidget *parent, int displacement)
	: QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint)
	, m_mousePressedInWindow(false)
{
	QSize windowSize = sizeHint();
	move(parent->pos().x() + parent->width() / 2 - 250, parent->pos().y() + parent->height() / 2 - 250);

	setAttribute(Qt::WA_TranslucentBackground, true);
	m_displacment = displacement;

	m_window = new QWidget(this);
	windowSize.setHeight(windowSize.height() - displacement - 10);
	windowSize.setWidth(windowSize.width() - 10);
	m_window->move(0, displacement);
	m_window->resize(windowSize);

	std::string frameStyle =
		"#SettingWindow {"
			"font-size: 17pt; "
			"border: 1px solid lightgray;"
			"border-radius: 15px; "
			"background: white; "
			"background-image: url(:/data/gui/icon/logo_1024_1024.png); "
		"}";
	m_window->setStyleSheet(frameStyle.c_str());
	m_window->setObjectName("SettingWindow");

	// window shadow
	if (QSysInfo::macVersion() == QSysInfo::MV_None)
	{
		QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
		effect->setBlurRadius(5);
		effect->setXOffset(2);
		effect->setYOffset(2);
		effect->setColor(Qt::darkGray);
		m_window->setGraphicsEffect(effect);
	}

	this->raise();
}

void QtSettingsWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape)
	{
		this->hide();
	}
}

QSize QtSettingsWindow::sizeHint() const
{
	return QSize(500, 500);
}

void QtSettingsWindow::resizeEvent(QResizeEvent *event)
{
	QSize windowSize = event->size()-QSize(10, 10 + m_displacment);
	m_window->resize(windowSize);
	m_window->move(0, m_displacment);
}

void QtSettingsWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton && m_mousePressedInWindow)
	{
		move(event->globalPos() - m_dragPosition);
		event->accept();
	}
}

void QtSettingsWindow::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = true;
	}
}

void QtSettingsWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
		m_mousePressedInWindow = false;
	}
}
