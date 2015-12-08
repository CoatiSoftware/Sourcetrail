#ifndef QT_STATUS_BAR_VIEW_H
#define QT_STATUS_BAR_VIEW_H

#include <memory>
#include <string>

#include "component/view/StatusBarView.h"
#include "qt/element/QtStatusBar.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtStatusBarView
	: public StatusBarView
{
public:
	QtStatusBarView(ViewLayout* viewLayout);
	~QtStatusBarView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// StatusBar view implementation
	virtual void showMessage(const std::string& message, bool isError, bool showLoader);
	virtual void setErrorCount(size_t count);

private:
	void doShowMessage(const std::string& message, bool isError, bool showLoader);
	void doSetErrorCount(size_t count);

	QtThreadedFunctor<const std::string&, bool, bool> m_showMessageFunctor;
	QtThreadedFunctor<size_t> m_setErrorCountFunctor;

	std::shared_ptr<QtStatusBar> m_widget;
};

#endif // !QT_STATUS_BAR_VIEW_H
