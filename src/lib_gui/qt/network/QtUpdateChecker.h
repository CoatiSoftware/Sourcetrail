#ifndef QT_UPDATE_CHECKER_H
#define QT_UPDATE_CHECKER_H

#include "UpdateChecker.h"

class QtUpdateChecker
	: public UpdateChecker
{
public:
	static void check(bool force = false);

	virtual void checkUpdate() override;
};

#endif // QT_UPDATE_CHECKER_H
