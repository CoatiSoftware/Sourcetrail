#include "component/view/LogView.h"

LogView::LogView(ViewLayout* viewLayout)
	: View(viewLayout)
{
}

LogView::~LogView()
{
}

std::string LogView::getName() const
{
	return "Logs";
}
