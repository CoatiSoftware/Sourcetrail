#ifndef QT_STATUS_BAR_VIEW_H
#define QT_STATUS_BAR_VIEW_H

#include <memory>
#include <string>

#include "component/view/StatusBarView.h"
#include "qt/element/QtStatusBar.h"
#include "qt/utility/QtThreadedFunctor.h"

class QtStatusBarView : public StatusBarView
{
public:
	QtStatusBarView(ViewLayout* viewLayout);
	~QtStatusBarView();

	// View implementation
	virtual void createWidgetWrapper();
	virtual void initView();
	virtual void refreshView();

	// StatusBar view implementation
	virtual void showMessage(const std::string& message, bool isError);
private:
	void doShowMessage(const std::string& message, bool isError);
	std::shared_ptr<QtStatusBar> m_widget;

	QtThreadedFunctor<const std::string&, bool> m_showMessageFunctor;
};

#endif // !QT_STATUS_BAR_VIEW_H
