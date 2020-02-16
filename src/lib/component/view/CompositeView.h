#ifndef COMPOSITE_VIEW_H
#define COMPOSITE_VIEW_H

#include <vector>

#include "MessageFocusedSearchView.h"
#include "MessageListener.h"
#include "View.h"
#include "ViewLayout.h"

class CompositeView
	: public View
	, public ViewLayout
	, public MessageListener<MessageFocusedSearchView>
{
public:
	enum CompositeDirection
	{
		DIRECTION_HORIZONTAL,
		DIRECTION_VERTICAL
	};

	CompositeView(ViewLayout* viewLayout, CompositeDirection direction, const std::string& name, Id tabId);
	virtual ~CompositeView();

	Id getSchedulerId() const override;

	CompositeDirection getDirection() const;
	const std::vector<View*>& getViews() const;

	virtual void addViewWidget(View* view) = 0;

	virtual void showFocusIndicator(bool focus) = 0;

	// View implementation
	virtual std::string getName() const override;

	// ViewLayout implementation
	void addView(View* view) override;
	void removeView(View* view) override;

	void showView(View* view) override;
	void hideView(View* view) override;

	void setViewEnabled(View* view, bool enabled) override;

private:
	void handleMessage(MessageFocusedSearchView* message) override;

	std::vector<View*> m_views;
	CompositeDirection m_direction;
	const std::string m_name;
	const Id m_tabId;
};

#endif	  // COMPOSITE_VIEW_H
