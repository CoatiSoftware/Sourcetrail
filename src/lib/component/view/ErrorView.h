#ifndef ERROR_VIEW_H
#define ERROR_VIEW_H

#include "component/view/View.h"
#include "data/ErrorInfo.h"

class ErrorView
	: public View
{
public:
	ErrorView(ViewLayout* viewLayout);
	virtual ~ErrorView();

	virtual std::string getName() const;

	virtual void clear() = 0;

	virtual void addErrors(const std::vector<ErrorInfo>& errors, bool scrollTo) = 0;
	virtual void setErrorId(Id errorId) = 0;
};

#endif // ERROR_VIEW_H
