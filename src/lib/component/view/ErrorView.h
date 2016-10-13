#ifndef ERROR_VIEW_H
#define ERROR_VIEW_H

#include "component/view/View.h"
#include "data/StorageTypes.h"

class ErrorView
	: public View
{
public:
	ErrorView(ViewLayout* viewLayout);
	virtual ~ErrorView();

	virtual std::string getName() const;

	virtual void showDockWidget();

	virtual void clear() = 0;

	virtual void addError(const StorageError& error) = 0;
	virtual void setErrorId(Id errorId) = 0;
};

#endif // ERROR_VIEW_H
