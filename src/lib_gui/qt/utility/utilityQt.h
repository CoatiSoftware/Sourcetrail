#ifndef UTILITY_QT_H
#define UTILITY_QT_H

#include <string>

class FilePath;
class QColor;
class QIcon;
class QPixmap;
class QString;
class QWidget;
class QtMainWindow;
class ViewLayout;

namespace utility
{
void setWidgetBackgroundColor(QWidget* widget, const std::string& color);
void setWidgetRetainsSpaceWhenHidden(QWidget* widget);

void loadFontsFromDirectory(const FilePath& path, const std::wstring& extension = L".otf");

std::string getStyleSheet(const FilePath& path);

QPixmap colorizePixmap(const QPixmap& pixmap, QColor color);
QIcon createButtonIcon(const FilePath& iconPath, const std::string& colorId);

QtMainWindow* getMainWindowforMainView(ViewLayout* viewLayout);

void copyNewFilesFromDirectory(const QString& src, const QString& dst);
}	 // namespace utility

#endif	  // UTILITY_QT_H
