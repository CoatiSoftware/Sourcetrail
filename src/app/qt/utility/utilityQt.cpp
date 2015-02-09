#include "utilityQt.h"

#include <set>

#include <QFile>
#include <QFontDatabase>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"

namespace utility
{
	void setWidgetBackgroundColor(QWidget* widget, const Colori& color)
	{
		QPalette palette = widget->palette();
		palette.setColor(widget->backgroundRole(), QColor(color.r, color.g, color.b, color.a));
		widget->setPalette(palette);
		widget->setAutoFillBackground(true);
	}

	void loadFontsFromDirectory(const std::string& path, const std::string& extension)
	{
		std::vector<std::string> extensions;
		extensions.push_back(extension);
		std::vector<std::string> fontFileNames = FileSystem::getFileNamesFromDirectory(path, extensions);

		std::set<int> loadedFontIds;

		for (const std::string& fontFileName: fontFileNames)
		{
			QFile file(fontFileName.c_str());
			if (file.open(QIODevice::ReadOnly))
			{
				int id = QFontDatabase::addApplicationFontFromData(file.readAll());
				if (id != -1)
				{
					loadedFontIds.insert(id);
				}
			}
		}

		for (int loadedFontId: loadedFontIds)
		{
			for (QString family: QFontDatabase::applicationFontFamilies(loadedFontId))
			{
				LOG_INFO("Loaded FontFamily: " + family.toStdString());
			}
		}
	}
}
