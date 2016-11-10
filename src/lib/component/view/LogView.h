#ifndef LOG_VIEW_H
#define LOG_VIEW_H

#include "component/view/View.h"

#include "utility/logging/LogMessage.h"
#include "utility/logging/Logger.h"

class LogView
	: public View
{
public:
	LogView(ViewLayout* viewLayout);
	virtual ~LogView();

	virtual std::string getName() const;

	virtual void clear() = 0;
	virtual void addLog(Logger::LogLevel type, const LogMessage& message) = 0;
};

#endif // LOG_VIEW_H
