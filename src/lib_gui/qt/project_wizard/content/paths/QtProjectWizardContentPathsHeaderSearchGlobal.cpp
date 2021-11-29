#include "QtProjectWizardContentPathsHeaderSearchGlobal.h"

#include <QMessageBox>

#include "ApplicationSettings.h"
#include "ResourcePaths.h"
#include "utilityApp.h"
#include "utilityPathDetection.h"

QtProjectWizardContentPathsHeaderSearchGlobal::QtProjectWizardContentPathsHeaderSearchGlobal(
	QtProjectWizardWindow* window)
	: QtProjectWizardContentPaths(
		  std::shared_ptr<SourceGroupSettings>(),
		  window,
		  QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY,
		  true)
{
	setTitleString(QStringLiteral("Global Include Paths"));
	setHelpString(QString::fromStdString(
		"The Global Include Paths will be used in all your projects in addition to the project "
		"specific Include Paths. "
		"These paths are usually passed to the compiler with the '-isystem' flag.<br />"
		"<br />"
		"Use them to add system header paths (See <a "
		"href=\"" +
		utility::getDocumentationLink() +
		"#finding-system-header-locations\">"
		"Finding System Header Locations</a> or use the auto detection below)."));

	m_pathDetector = utility::getCxxHeaderPathDetector();
	m_makePathsRelativeToProjectFileLocation = false;
}

void QtProjectWizardContentPathsHeaderSearchGlobal::load()
{
	setPaths(ApplicationSettings::getInstance()->getHeaderSearchPaths());
}

void QtProjectWizardContentPathsHeaderSearchGlobal::save()
{
	std::vector<FilePath> paths;
	for (const FilePath& headerPath: m_list->getPathsAsDisplayed())
	{
		if (headerPath != ResourcePaths::getCxxCompilerHeaderDirectoryPath())
		{
			paths.push_back(headerPath);
		}
	}

	ApplicationSettings::getInstance()->setHeaderSearchPaths(paths);
	ApplicationSettings::getInstance()->save();
}

bool QtProjectWizardContentPathsHeaderSearchGlobal::check()
{
	if (utility::getOsType() == OS_WINDOWS)
	{
		return QtProjectWizardContentPaths::check();
	}

	std::vector<FilePath> paths;
	QString compilerHeaderPaths;
	for (const FilePath& headerPath: m_list->getPathsAsDisplayed())
	{
		if (headerPath != ResourcePaths::getCxxCompilerHeaderDirectoryPath() &&
			headerPath.getCanonical().getConcatenated(L"/stdarg.h").exists())
		{
			compilerHeaderPaths += QString::fromStdWString(headerPath.wstr()) + "\n";
		}
		else
		{
			paths.push_back(headerPath);
		}
	}

	if (compilerHeaderPaths.size())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(QStringLiteral("Multiple Compiler Headers"));
		msgBox.setInformativeText(
			"Your Global Include Paths contain other paths that hold C/C++ compiler headers, "
			"probably those of your local C/C++ compiler. They are possibly in conflict with the "
			"compiler headers of "
			"Sourcetrail's C/C++ indexer. This can lead to compatibility errors during indexing. "
			"Do "
			"you want to remove "
			"these paths?");
		msgBox.setDetailedText(compilerHeaderPaths);
		msgBox.addButton(QStringLiteral("Remove"), QMessageBox::ButtonRole::YesRole);
		msgBox.addButton(QStringLiteral("Keep"), QMessageBox::ButtonRole::NoRole);
		msgBox.setIcon(QMessageBox::Icon::Question);
		int ret = msgBox.exec();

		if (ret == 0)	 // QMessageBox::Yes
		{
			setPaths(paths);
			save();
		}
	}

	return QtProjectWizardContentPaths::check();
}

void QtProjectWizardContentPathsHeaderSearchGlobal::detectedPaths(const std::vector<FilePath>& paths)
{
	std::vector<FilePath> headerPaths;
	for (const FilePath& headerPath: paths)
	{
		if (headerPath != ResourcePaths::getCxxCompilerHeaderDirectoryPath())
		{
			headerPaths.push_back(headerPath);
		}
	}
	setPaths(headerPaths);
}

void QtProjectWizardContentPathsHeaderSearchGlobal::setPaths(const std::vector<FilePath>& paths)
{
	// check data change to avoid UI update that messes with the scroll position
	{
		std::vector<FilePath> currentPaths = m_list->getPathsAsDisplayed();
		if (currentPaths.size())
		{
			currentPaths.erase(currentPaths.begin());
		}

		if (currentPaths.size() == paths.size())
		{
			bool same = true;
			for (size_t i = 0; i < paths.size(); ++i)
			{
				if (currentPaths[i] != paths[i])
				{
					same = false;
					break;
				}
			}

			if (same)
			{
				return;
			}
		}
	}

	m_list->setPaths({});
	m_list->addPaths({ResourcePaths::getCxxCompilerHeaderDirectoryPath()}, true);
	m_list->addPaths(paths);
}
