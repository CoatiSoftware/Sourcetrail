#include "utility/path_detector/java_runtime/JavaPathDetectorWindows.h"

#include <QSettings>
#include <QSysInfo>

JavaPathDetectorWindows::JavaPathDetectorWindows(const std::string javaVersion)
	: JavaPathDetector("Java " + javaVersion + " for Windows", javaVersion)
{
}

JavaPathDetectorWindows::~JavaPathDetectorWindows()
{
}

std::vector<FilePath> JavaPathDetectorWindows::getPaths() const
{
	QString key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\";

	key += "Wow6432Node\\"; // we need java 32 bit

	key += ("JavaSoft\\Java Runtime Environment\\" + m_javaVersion).c_str();

	QSettings expressKey(key, QSettings::NativeFormat); // NativeFormat means from Registry on Windows.
	QString value = expressKey.value("RuntimeLib").toString();

	FilePath path(value.toStdString());

	std::vector<FilePath> paths;
	if (path.exists())
	{
		paths.push_back(path.parentDirectory());
	}
	return paths;
}
