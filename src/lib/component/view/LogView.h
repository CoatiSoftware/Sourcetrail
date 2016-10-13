#ifndef LOG_VIEW_H
#define LOG_VIEW_H

#include "component/view/View.h"

class LogView
	: public View
{
public:
	LogView(ViewLayout* viewLayout);
	virtual ~LogView();

	virtual std::string getName() const;

	virtual void clear() = 0;
};

#endif // LOG_VIEW_H
