#include "QtCompositeView.h"

#include <QBoxLayout>

#include "ColorScheme.h"
#include "QtViewWidgetWrapper.h"
#include "utilityQt.h"

QtCompositeView::QtCompositeView(
	ViewLayout* viewLayout, CompositeDirection direction, const std::string& name, Id tabId)
	: CompositeView(viewLayout, direction, name, tabId)
{
	QBoxLayout* topLayout = new QVBoxLayout();
	topLayout->setSpacing(0);
	topLayout->setContentsMargins(0, 0, 0, 0);

	const size_t indicatorHeight = 3;

	{
		m_focusIndicator = new QWidget();
		m_focusIndicator->setObjectName(QStringLiteral("focus_indicator"));
		m_focusIndicator->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		m_focusIndicator->setFixedHeight(indicatorHeight);
		topLayout->addWidget(m_focusIndicator);
	}

	if (getDirection() == CompositeView::DIRECTION_HORIZONTAL)
	{
		m_layout = new QHBoxLayout();
	}
	else
	{
		m_layout = new QVBoxLayout();
	}

	m_layout->setSpacing(5);
	m_layout->setContentsMargins(8, 8 - indicatorHeight, 8, 7);
	m_layout->setAlignment(Qt::AlignTop);

	topLayout->addLayout(m_layout);

	m_widget = new QWidget();
	m_widget->setLayout(topLayout);

	refreshView();
}

void QtCompositeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtCompositeView::refreshView()
{
	m_onQtThread([=]() {
		utility::setWidgetBackgroundColor(
			m_widget, ColorScheme::getInstance()->getColor("search/background"));
	});

	showFocusIndicator(false);
}

void QtCompositeView::addViewWidget(View* view)
{
	m_layout->addWidget(QtViewWidgetWrapper::getWidgetOfView(view));
}

void QtCompositeView::showFocusIndicator(bool focus)
{
	m_onQtThread([=]() {
		const std::string& colorName = focus ? "window/focus" : "search/background";
		utility::setWidgetBackgroundColor(m_focusIndicator, ColorScheme::getInstance()->getColor(colorName));
	});
}
