#include "utilityQt.h"

#include <set>

#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QIcon>
#include <QPainter>
#include <QWidget>

#include "FilePath.h"
#include "FileSystem.h"
#include "QtMainView.h"
#include "ResourcePaths.h"
#include "TextAccess.h"
#include "logging.h"
#include "utilityApp.h"
#include "utilityString.h"

#include "ApplicationSettings.h"
#include "ColorScheme.h"

namespace utility
{
void setWidgetBackgroundColor(QWidget* widget, const std::string& color)
{
	QPalette palette = widget->palette();
	palette.setColor(widget->backgroundRole(), QColor(color.c_str()));
	widget->setPalette(palette);
	widget->setAutoFillBackground(true);
}

void setWidgetRetainsSpaceWhenHidden(QWidget* widget)
{
	QSizePolicy pol = widget->sizePolicy();
	pol.setRetainSizeWhenHidden(true);
	widget->setSizePolicy(pol);
}

void loadFontsFromDirectory(const FilePath& path, const std::wstring& extension)
{
	std::vector<std::wstring> extensions;
	extensions.push_back(extension);
	std::vector<FilePath> fontFilePaths = FileSystem::getFilePathsFromDirectory(path, extensions);

	std::set<int> loadedFontIds;

	for (const FilePath& fontFilePath: fontFilePaths)
	{
		QFile file(QString::fromStdWString(fontFilePath.wstr()));
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
		for (QString& family: QFontDatabase::applicationFontFamilies(loadedFontId))
		{
			LOG_INFO(L"Loaded FontFamily: " + family.toStdWString());
		}
	}
}

std::string getStyleSheet(const FilePath& path)
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
			LOG_ERROR(L"Syntax error in file: " + path.wstr());
			return "";
		}

		const std::string key = seq.front();
		std::string val = seq.back();

		if (key == "setting")
		{
			if (val.find("font_size") != std::string::npos)
			{
				// check for modifier
				if (val.find('+') != std::string::npos)
				{
					const size_t findPos = val.find('+');
					std::string sub = val.substr(findPos + 1);

					int mod = std::stoi(sub);

					val = std::to_string(ApplicationSettings::getInstance()->getFontSize() + mod);
				}
				else if (val.find('-') != std::string::npos)
				{
					const size_t findPos = val.find('-');
					std::string sub = val.substr(findPos + 1);

					int mod = std::stoi(sub);

					val = std::to_string(ApplicationSettings::getInstance()->getFontSize() - mod);
				}
				else if (val.find('*') != std::string::npos)
				{
					const size_t findPos = val.find('*');
					std::string sub = val.substr(findPos + 1);

					int mod = std::stoi(sub);

					val = std::to_string(ApplicationSettings::getInstance()->getFontSize() * mod);
				}
				else if (val.find('/') != std::string::npos)
				{
					const size_t findPos = val.find('/');
					std::string sub = val.substr(findPos + 1);

					int mod = std::stoi(sub);

					val = std::to_string(ApplicationSettings::getInstance()->getFontSize() / mod);
				}
				else
				{
					val = std::to_string(ApplicationSettings::getInstance()->getFontSize());
				}
			}
			else if (val == "font_name")
			{
				val = ApplicationSettings::getInstance()->getFontName();
			}
			else if (val == "gui_path")
			{
				val = ResourcePaths::getGuiPath().str();

				size_t index = 0;
				while (true)
				{
					index = val.find('\\', index);
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
				LOG_ERROR(L"Syntax error in file: " + path.wstr());
				return "";
			}
		}
		else if (key == "color")
		{
			if (!ColorScheme::getInstance()->hasColor(val))
			{
				LOG_WARNING(
					"Color scheme does not provide value for key \"" + val +
					"\" requested by style \"" + path.str() + "\".");
			}
			val = ColorScheme::getInstance()->getColor(val);
		}
		else if (key == "platform_wml")
		{
			std::vector<std::string> values = utility::splitToVector(val, '|');
			if (values.size() != 3)
			{
				LOG_ERROR(L"Syntax error in file: " + path.wstr());
				return "";
			}

			switch (utility::getOsType())
			{
			case OS_WINDOWS:
				val = values[0];
				break;
			case OS_MAC:
				val = values[1];
				break;
			case OS_LINUX:
				val = values[2];
				break;
			default:
				break;
			}
		}
		else
		{
			LOG_ERROR(L"Syntax error in file: " + path.wstr());
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

QIcon createButtonIcon(const FilePath& iconPath, const std::string& colorId)
{
	ColorScheme* scheme = ColorScheme::getInstance().get();

	QPixmap pixmap(QString::fromStdWString(iconPath.wstr()));
	QIcon icon(utility::colorizePixmap(pixmap, scheme->getColor(colorId + "/icon").c_str()));

	if (scheme->hasColor(colorId + "/icon_disabled"))
	{
		icon.addPixmap(
			utility::colorizePixmap(pixmap, scheme->getColor(colorId + "/icon_disabled").c_str()),
			QIcon::Disabled);
	}

	return icon;
}

QtMainWindow* getMainWindowforMainView(ViewLayout* viewLayout)
{
	if (QtMainView* mainView = dynamic_cast<QtMainView*>(viewLayout))
	{
		return mainView->getMainWindow();
	}
	return nullptr;
}

void copyNewFilesFromDirectory(const QString& src, const QString& dst)
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
}	 // namespace utility
