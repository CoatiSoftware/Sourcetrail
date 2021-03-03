#include "JavaPathDetectorWindows.h"

#include <QSettings>
#include <QSysInfo>

#include "FilePath.h"
#include "utilityApp.h"
#include "utilityString.h"

JavaPathDetectorWindows::JavaPathDetectorWindows(const std::string javaVersion, bool isJre)
	: JavaPathDetector(
		  "Java " + std::string(isJre ? "JRE" : "JDK") + " " + javaVersion + " for Windows",
		  javaVersion)
	, m_isJre(isJre)
{
}

std::vector<FilePath> JavaPathDetectorWindows::doGetPaths() const
{
	QString key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\";

	if (utility::getApplicationArchitectureType() == APPLICATION_ARCHITECTURE_X86_32)
	{
		key += "Wow6432Node\\";
	}

	key += "JavaSoft\\";

	if (m_isJre)
	{
		if (utility::isPrefix(std::string("1."), m_javaVersion))
		{
			key += ("Java Runtime Environment\\" + m_javaVersion).c_str();
		}
		else
		{
			key += ("JRE\\" + m_javaVersion).c_str();
		}

		// NativeFormat means from Registry on Windows.
		QSettings settings(key, QSettings::NativeFormat);
		const QString value = settings.value("RuntimeLib").toString();

		const FilePath path(value.toStdWString());
		if (path.exists())
		{
			return {path};
		}
	}
	else
	{
		key += "JDK";
		{
			const QSettings settings(key, QSettings::NativeFormat);
			for (const QString& child: settings.childGroups())
			{
				if (child.startsWith(QString::fromStdString(m_javaVersion)))
				{
					key += "\\" + child;
				}
			}
		}
		{
			const QSettings settings(key, QSettings::NativeFormat);
			const QString value = settings.value("JavaHome").toString();

			const FilePath path = FilePath(value.toStdWString()).concatenate(L"bin/server/jvm.dll");
			if (path.exists())
			{
				return {path};
			}
		}
	}
	return {};
}
