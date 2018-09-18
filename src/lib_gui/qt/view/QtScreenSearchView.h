#ifndef QT_SCREEN_SEARCH_VIEW_H
#define QT_SCREEN_SEARCH_VIEW_H

#include "ControllerProxy.h"
#include "ScreenSearchController.h"
#include "ScreenSearchView.h"
#include "QtThreadedFunctor.h"

class QtScreenSearchBox;
class QToolBar;

class QtScreenSearchView
	: public QObject
	, public ScreenSearchView
{
	Q_OBJECT

public:
	QtScreenSearchView(ViewLayout* viewLayout);
	~QtScreenSearchView();

	// View implementation
	virtual void createWidgetWrapper() override;
	virtual void initView() override;
	virtual void refreshView() override;

	// ScreenSearchView implementation
	virtual void setMatchCount(size_t matchCount) override;
	virtual void setMatchIndex(size_t matchIndex) override;

	virtual void addResponder(const std::string& name) override;

public slots:
	void show();
	void hide();

private:
	ControllerProxy<ScreenSearchController> m_controllerProxy;
	QtThreadedLambdaFunctor m_onQtThread;

	QtScreenSearchBox* m_widget;
	QToolBar* m_bar;
};

#endif // QT_SCREEN_SEARCH_VIEW_H
