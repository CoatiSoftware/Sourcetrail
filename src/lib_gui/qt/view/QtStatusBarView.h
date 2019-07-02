#ifndef QT_STATUS_BAR_VIEW_H
#define QT_STATUS_BAR_VIEW_H

#include <memory>
#include <string>

#include "StatusBarView.h"
#include "ErrorCountInfo.h"
#include "QtStatusBar.h"
#include "QtThreadedFunctor.h"

class QtStatusBarView
	: public StatusBarView
{
public:
	QtStatusBarView(ViewLayout* viewLayout);
	~QtStatusBarView() = default;

	// View implementation
	void createWidgetWrapper() override;
	void refreshView() override;

	// StatusBar view implementation
	void showMessage(const std::wstring& message, bool isError, bool showLoader) override;
	void setErrorCount(ErrorCountInfo errorCount) override;

	void showIdeStatus(const std::wstring& message) override;

	void showIndexingProgress(size_t progressPercent) override;
	void hideIndexingProgress() override;

private:
	QtThreadedLambdaFunctor m_onQtThread;

	QtStatusBar* m_widget;
};

#endif // !QT_STATUS_BAR_VIEW_H
