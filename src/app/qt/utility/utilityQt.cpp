#include "utilityQt.h"

namespace utility
{
	void setWidgetBackgroundColor(QWidget* widget, const Colori& color)
	{
		QPalette palette = widget->palette();
		palette.setColor(widget->backgroundRole(), QColor(color.r, color.g, color.b, color.a));
		widget->setPalette(palette);
		widget->setAutoFillBackground(true);
	}
}
