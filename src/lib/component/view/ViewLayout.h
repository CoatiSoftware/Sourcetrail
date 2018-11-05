#ifndef VIEW_LAYOUT_H
#define VIEW_LAYOUT_H

#include <string>

class View;

class ViewLayout
{
public:
	virtual ~ViewLayout() = default;

	virtual void addView(View* view) = 0;
	virtual void overrideView(View* view);
	virtual void removeView(View* view) = 0;

	virtual void showView(View* view) = 0;
	virtual void hideView(View* view) = 0;

	virtual void setViewEnabled(View* view, bool enabled) = 0;

	virtual View* findFloatingView(const std::string& name) const;

	virtual void showOriginalViews();
};

#endif // VIEW_LAYOUT_H
