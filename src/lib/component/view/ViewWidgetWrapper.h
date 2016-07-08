#ifndef VIEW_WIDGET_WRAPPER_H
#define VIEW_WIDGET_WRAPPER_H

class ViewWidgetWrapper
{
public:
	ViewWidgetWrapper();
	virtual ~ViewWidgetWrapper();

	virtual void createOverlay() = 0;
	virtual void showOverlay() = 0;
	virtual void hideOverlay() = 0;
};

#endif // VIEW_WIDGET_WRAPPER_H
