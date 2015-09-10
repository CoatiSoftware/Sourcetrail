#ifndef COMPOSITE_VIEW_H
#define COMPOSITE_VIEW_H

#include <vector>

#include "component/view/View.h"
#include "component/view/ViewLayout.h"

class CompositeView
	: public View
	, public ViewLayout
{
public:
	enum CompositeDirection
	{
		DIRECTION_HORIZONTAL,
		DIRECTION_VERTICAL
	};

	CompositeView(ViewLayout* viewLayout, CompositeDirection direction, const std::string& name);
	virtual ~CompositeView();

	CompositeDirection getDirection() const;
	const std::vector<View*>& getViews() const;

	virtual void addViewWidget(View* view) = 0;

	// View implementation
	virtual std::string getName() const;

	// ViewLayout implementation
	virtual void addView(View* view);
	virtual void removeView(View* view);

	virtual void showView(View* view);
	virtual void hideView(View* view);

	virtual void loadLayout();
	virtual void saveLayout();

private:
	std::vector<View*> m_views;
	CompositeDirection m_direction;
	std::string m_name;
};

#endif // COMPOSITE_VIEW_H
