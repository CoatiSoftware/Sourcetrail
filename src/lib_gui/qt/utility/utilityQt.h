#ifndef UTILITY_QT_H
#define UTILITY_QT_H

#include <memory>
#include <qwidget.h>

namespace utility
{
	void setWidgetBackgroundColor(QWidget* widget, const std::string& color);
	void loadFontsFromDirectory(const std::string& path, const std::string& extension = ".otf");

	std::string getStyleSheet(const std::string& path);

	QPixmap colorizePixmap(const QPixmap& pixmap, QColor color);
}

# endif // UTILITY_QT_H
