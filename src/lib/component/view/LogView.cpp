#include "component/view/LogView.h"

const int LogView::LogLimit = 500;

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

bool LogView::hasLogLevel(const Logger::LogLevel type, const Logger::LogLevelMask mask) const
{
	return mask & type;
}
