#ifndef UTILITY_QT_H
#define UTILITY_QT_H

#include <memory>
#include <qwidget.h>

#include "utility/math/Color.h"

namespace utility
{
	void setWidgetBackgroundColor(std::shared_ptr<QWidget> widget, const Colori& color);
}

# endif // UTILITY_QT_H
