#include "QtTooltipView.h"

#include "QtMainView.h"
#include "QtMainWindow.h"
#include "QtTooltip.h"
#include "QtViewWidgetWrapper.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtTooltipView::QtTooltipView(ViewLayout* viewLayout): TooltipView(viewLayout)
{
	m_widget = new QtTooltip(utility::getMainWindowforMainView(viewLayout));
}

void QtTooltipView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtTooltipView::refreshView()
{
	m_onQtThread([=]() {
		m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(
														   L"tooltip_view/tooltip_view.css"))
									.c_str());
	});
}

void QtTooltipView::showTooltip(const TooltipInfo& info, const View* parent)
{
	m_onQtThread([=]() {
		if (m_widget->isHovered())
		{
			return;
		}

		m_widget->hide();
		m_widget->setTooltipInfo(info);

		if (parent)
		{
			m_widget->setParentView(QtViewWidgetWrapper::getWidgetOfView(parent)->parentWidget());
		}

		m_widget->show();
	});
}

void QtTooltipView::hideTooltip(bool force)
{
	m_onQtThread([=]() { m_widget->hide(force); });
}

bool QtTooltipView::tooltipVisible() const
{
	return m_widget->isVisible();
}
