#ifndef STATUS_VIEW_H
#define STATUS_VIEW_H

#include "component/view/View.h"

#include "utility/Status.h"
#include "utility/logging/LogMessage.h"
#include "utility/logging/Logger.h"


class StatusView
	: public View
{
public:
	StatusView(ViewLayout* viewLayout);
	virtual ~StatusView();

	virtual std::string getName() const;

	virtual void addStatus(const std::vector<Status>& status) = 0;
	virtual void clear() = 0;
};

#endif // STATUS_VIEW_H
