#ifndef REFRESH_CONTROLLER_H
#define REFRESH_CONTROLLER_H

#include "Controller.h"

class RefreshView;

class RefreshController
	: public Controller
{
public:
	RefreshController();
	virtual ~RefreshController();

	virtual void clear();

private:
	RefreshView* getView();
};

#endif // REFRESH_CONTROLLER_H
