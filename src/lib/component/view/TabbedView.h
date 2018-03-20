#ifndef TABBED_VIEW_H
#define TABBED_VIEW_H

#include <vector>

#include "component/view/View.h"
#include "component/view/ViewLayout.h"

class TabbedView
	: public View
	, public ViewLayout
{
public:
	TabbedView(ViewLayout* viewLayout, const std::string& name);
	virtual ~TabbedView() = default;

	const std::vector<View*>& getViews() const;

	virtual void addViewWidget(View* view) = 0;

	// View implementation
	virtual std::string getName() const;

	// ViewLayout implementation
	virtual void addView(View* view);
	virtual void removeView(View* view);

	virtual void showView(View* view);
	virtual void hideView(View* view);

private:
	std::vector<View*> m_views;
	std::string m_name;
};

#endif // TABBED_VIEW_H
