#ifndef ERROR_VIEW_H
#define ERROR_VIEW_H

#include <vector>

#include "ErrorCountInfo.h"
#include "ErrorFilter.h"
#include "ErrorInfo.h"
#include "View.h"

class ErrorView: public View
{
public:
	ErrorView(ViewLayout* viewLayout);
	virtual ~ErrorView();

	virtual std::string getName() const;

	virtual void clear() = 0;

	virtual void addErrors(
		const std::vector<ErrorInfo>& errors, const ErrorCountInfo& errorCount, bool scrollTo) = 0;
	virtual void setErrorId(Id errorId) = 0;

	virtual ErrorFilter getErrorFilter() const = 0;
	virtual void setErrorFilter(const ErrorFilter& filter) = 0;
};

#endif	  // ERROR_VIEW_H
