#ifndef UPDATE_CHECKER_H
#define UPDATE_CHECKER_H

class UpdateChecker
{
public:
	virtual ~UpdateChecker() = default;
	virtual void checkUpdate() = 0;
};

#endif	  // UPDATE_CHECKER_H
