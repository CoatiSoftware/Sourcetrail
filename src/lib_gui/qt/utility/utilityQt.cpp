#include "qt/utility/utilityQt.h"

#include <set>

#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QPainter>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/ResourcePaths.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

namespace utility
{
	void setWidgetBackgroundColor(QWidget* widget, const std::string& color)
	{
		QPalette palette = widget->palette();
		palette.setColor(widget->backgroundRole(), QColor(color.c_str()));
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

	std::string getStyleSheet(const std::string& path)
	{
		std::string css = TextAccess::createFromFile(path)->getText();

		size_t pos = 0;

		while (pos != std::string::npos)
		{
			size_t posA = css.find('<', pos);
			size_t posB = css.find('>', pos);

			if (posA == std::string::npos || posB == std::string::npos)
			{
				break;
			}

			std::deque<std::string> seq = utility::split(css.substr(posA + 1, posB - posA - 1), ':');
			if (seq.size() != 2)
			{
				LOG_ERROR("Syntax error in file: " + path);
				return "";
			}

			std::string key = seq.front();
			std::string val = seq.back();

			if (key == "setting")
			{
				if (val == "font_size")
				{
					val = std::to_string(ApplicationSettings::getInstance()->getFontSize());
				}
				else if (val == "font_size+2")
				{
					val = std::to_string(ApplicationSettings::getInstance()->getFontSize() + 2);
				}
				else if (val == "font_size+5")
				{
					val = std::to_string(ApplicationSettings::getInstance()->getFontSize() + 5);
				}
				else if (val == "font_size-2")
				{
					val = std::to_string(ApplicationSettings::getInstance()->getFontSize() - 2);
				}
				else if (val == "font_name")
				{
					val = ApplicationSettings::getInstance()->getFontName();
				}
				else if (val == "gui_path")
				{
					val = ResourcePaths::getGuiPath();

					size_t index = 0;
					while (true)
					{
						index = val.find("\\", index);
						if (index == std::string::npos)
						{
							break;
						}
						val.replace(index, 1, "/");
						index += 3;
					}
				}
				else
				{
					LOG_ERROR("Syntax error in file: " + path);
					return "";
				}
			}
			else if (key == "color")
			{
				val = ColorScheme::getInstance()->getColor(val);
			}
			else
			{
				LOG_ERROR("Syntax error in file: " + path);
				return "";
			}

			css.replace(posA, posB - posA + 1, val);
			pos = posA + val.size();
		}

		return css;
	}

	QPixmap colorizePixmap(const QPixmap& pixmap, QColor color)
	{
		QImage image = pixmap.toImage();
		QImage colorImage(image.size(), image.format());
		QPainter colorPainter(&colorImage);
		colorPainter.fillRect(image.rect(), color);

		QPainter painter(&image);
		painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
		painter.drawImage(0, 0, colorImage);
		return QPixmap::fromImage(image);
	}

	void copyNewFilesFromDirectory(QString src, QString dst)
	{
		QDir dir(src);
		if (!dir.exists())
		{
			return;
		}

		foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
		{
			QString dst_path = dst + QDir::separator() + d;

			dir.mkpath(dst_path);
			copyNewFilesFromDirectory(src + QDir::separator() + d, dst_path);
		}

		foreach (QString f, dir.entryList(QDir::Files))
		{
			if (!QFile::exists(dst + QDir::separator() + f))
			{
				QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
			}
		}
	}
}
