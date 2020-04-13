#ifndef STATUS_BAR_VIEW_H
#define STATUS_BAR_VIEW_H

#include "../../data/ErrorCountInfo.h"
#include "View.h"

class StatusBarController;

class StatusBarView: public View
{
public:
	StatusBarView(ViewLayout* viewLayout);
	virtual ~StatusBarView() = default;

	virtual std::string getName() const;
	virtual void showMessage(const std::wstring& message, bool isError, bool showLoader) = 0;
	virtual void setErrorCount(ErrorCountInfo errorCount) = 0;

	virtual void showIdeStatus(const std::wstring& message) = 0;

	virtual void showIndexingProgress(size_t progressPercent) = 0;
	virtual void hideIndexingProgress() = 0;

protected:
	StatusBarController* getController();
};

#endif	  // STATUS_BAR_VIEW_H
