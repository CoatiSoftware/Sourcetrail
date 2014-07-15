#ifndef UTILITY_QT_H
#define UTILITY_QT_H

#include <memory>
#include <qwidget.h>

#include "utility/math/Color.h"

namespace utility
{
	void setWidgetBackgroundColor(QWidget* widget, const Colori& color);
	void loadFontsFromDirectory(const std::string& path, const std::string& extension = ".otf");
}

# endif // UTILITY_QT_H
