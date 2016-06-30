#include "qt/view/QtViewOverlay.h"

#include <QWidget>
#include <QEvent>

ResizeFilter::ResizeFilter(QWidget* widget)
	: m_widget(widget)
{
}

bool ResizeFilter::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		QWidget* parent = dynamic_cast<QWidget*>(obj);
		m_widget->setGeometry(0, 0, parent->width(), parent->height());
	}

	return QObject::eventFilter(obj, event);
}


QtViewOverlay::QtViewOverlay(QWidget* parent)
	: m_parent(parent)
	, m_beforeFunctor(std::bind(&QtViewOverlay::doBefore, this))
	, m_afterFunctor(std::bind(&QtViewOverlay::doAfter, this))
{
	m_overlay = new QWidget(m_parent);
	m_parent->installEventFilter(new ResizeFilter(m_overlay));

	std::string frameStyle =
		"#overlay {"
			"background: #55FF0000; "
		"}";
	m_overlay->setStyleSheet(frameStyle.c_str());
	m_overlay->setObjectName("overlay");
	m_overlay->hide();
}

void QtViewOverlay::handleMessage(MessageLoadBefore* message)
{
	m_beforeFunctor();
}

void QtViewOverlay::handleMessage(MessageLoadAfter* message)
{
	m_afterFunctor();
}

void QtViewOverlay::doBefore()
{
	m_overlay->show();
}

void QtViewOverlay::doAfter()
{
	m_overlay->hide();
}
