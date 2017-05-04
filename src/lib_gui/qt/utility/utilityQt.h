#ifndef UTILITY_QT_H
#define UTILITY_QT_H

#include <string>

class QColor;
class QIcon;
class QPixmap;
class QString;
class QWidget;
class FilePath;

namespace utility
{
	void setWidgetBackgroundColor(QWidget* widget, const std::string& color);
	void setWidgetRetainsSpaceWhenHidden(QWidget* widget);

	void loadFontsFromDirectory(const FilePath& path, const std::string& extension = ".otf");

	std::string getStyleSheet(const FilePath& path);

	QPixmap colorizePixmap(const QPixmap& pixmap, QColor color);
	QIcon createButtonIcon(const std::string& iconPath, const std::string& colorId);

	void copyNewFilesFromDirectory(QString src, QString dst);
}

# endif // UTILITY_QT_H
