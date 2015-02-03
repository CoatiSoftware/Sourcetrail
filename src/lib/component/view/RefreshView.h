#ifndef REFRESH_VIEW_H
#define REFRESH_VIEW_H

#include "component/view/View.h"

class RefreshController;

class RefreshView
	: public View
{
public:
	RefreshView(ViewLayout* viewLayout);
	virtual ~RefreshView();

	virtual std::string getName() const;

private:
	RefreshController* getController();
};

#endif // REFRESH_VIEW_H
