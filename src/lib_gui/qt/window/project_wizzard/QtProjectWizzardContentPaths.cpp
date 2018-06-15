#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "component/view/DialogView.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "project/SourceGroupCxxEmpty.h"
#include "project/SourceGroupJavaEmpty.h"
#include "qt/view/QtDialogView.h"
#include "qt/window/QtPathListDialog.h"
#include "qt/window/QtSelectPathsDialog.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxx.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsCxxCodeblocks.h"
#include "settings/SourceGroupSettingsCxxSonargraph.h"
#include "settings/SourceGroupSettingsJavaEmpty.h"
#include "settings/SourceGroupSettingsWithClasspath.h"
#include "settings/SourceGroupSettingsWithIndexedHeaderPaths.h"
#include "settings/SourceGroupSettingsWithExcludeFilters.h"
#include "settings/SourceGroupSettingsWithSourceExtensions.h"
#include "settings/SourceGroupSettingsWithSourcePaths.h"
#include "utility/codeblocks/CodeblocksProject.h"
#include "utility/file/FileManager.h"
#include "utility/sonargraph/SonargraphProject.h"
#include "utility/CompilationDatabase.h"
#include "utility/IncludeDirective.h"
#include "utility/IncludeProcessing.h"
#include "utility/ScopedFunctor.h"
#include "utility/utility.h"
#include "utility/utilityFile.h"
#include "utility/utilityPathDetection.h"
#include "utility/utilityString.h"
#include "Application.h"

QtProjectWizzardContentPaths::QtProjectWizzardContentPaths(
	std::shared_ptr<SourceGroupSettings> settings,
	QtProjectWizzardWindow* window,
	QtPathListBox::SelectionPolicyType selectionPolicy,
	bool checkMissingPaths
)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_makePathsRelativeToProjectFileLocation(true)
	, m_selectionPolicy(selectionPolicy)
	, m_checkMissingPaths(checkMissingPaths)
{
}

void QtProjectWizzardContentPaths::populate(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel(m_titleString);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	if (m_helpString.size() > 0)
	{
		addHelpButton(m_titleString, m_helpString, layout, row);
	}

	m_list = new QtPathListBox(this, m_titleString, m_selectionPolicy);

	if (m_makePathsRelativeToProjectFileLocation && m_settings)
	{
		m_list->setRelativeRootDirectory(m_settings->getProjectDirectoryPath());
	}

	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	if (m_showFilesString.size() > 0)
	{
		addFilesButton(m_showFilesString, layout, row);
		row++;
	}

	if (m_pathDetector)
	{
		addDetection(layout, row);
		row++;
	}
}

bool QtProjectWizzardContentPaths::check()
{
	if (m_checkMissingPaths)
	{
		QString missingPaths;
		std::vector<FilePath> existingPaths;

		for (const FilePath& path : m_list->getPathsAsDisplayed())
		{
			std::vector<FilePath> expandedPaths(1, path);
			if (m_settings)
			{
				expandedPaths = m_settings->makePathsExpandedAndAbsolute(expandedPaths);
			}

			size_t existingCount = 0;
			for (const FilePath& expandedPath : expandedPaths)
			{
				if (!expandedPath.exists())
				{
					missingPaths.append(QString::fromStdWString(expandedPath.wstr() + L"\n"));
				}
				else
				{
					existingCount++;
				}
			}

			if (!expandedPaths.empty() && expandedPaths.size() == existingCount)
			{
				existingPaths.push_back(path);
			}
		}

		if (!missingPaths.isEmpty())
		{
			QMessageBox msgBox;
			msgBox.setText(
				QString(
					"Some provided paths do not exist at \"%1\". Do you want to remove them before continuing?"
				).arg(m_titleString)
			);
			msgBox.setDetailedText(missingPaths);
			QPushButton* removeButton = msgBox.addButton("Remove", QMessageBox::YesRole);
			QPushButton* keepButton = msgBox.addButton("Keep", QMessageBox::ButtonRole::NoRole);
			QPushButton* cancelButton = msgBox.addButton("Cancel", QMessageBox::ButtonRole::RejectRole);

			msgBox.exec();

			if (msgBox.clickedButton() == removeButton)
			{
				m_list->setPaths(existingPaths);
				save();
			}
			else if (msgBox.clickedButton() == keepButton)
			{
				return true;
			}
			else if (msgBox.clickedButton() == cancelButton)
			{
				return false;
			}
		}
	}
	return true;
}

void QtProjectWizzardContentPaths::setTitleString(const QString& title)
{
	m_titleString = title;
}

void QtProjectWizzardContentPaths::setHelpString(const QString& help)
{
	m_helpString = help;
}

void QtProjectWizzardContentPaths::addDetection(QGridLayout* layout, int row)
{
	std::vector<std::string> detectorNames = m_pathDetector->getWorkingDetectorNames();
	if (!detectorNames.size())
	{
		return;
	}

	QLabel* label = new QLabel("Auto detection from:");

	m_detectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_detectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton("detect");
	button->setObjectName("windowButton");
	connect(button, &QPushButton::clicked, this, &QtProjectWizzardContentPaths::detectionClicked);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_detectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(detectionWidget, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
}

void QtProjectWizzardContentPaths::detectionClicked()
{
	std::vector<FilePath> paths = m_pathDetector->getPaths(m_detectorBox->currentText().toStdString());
	std::vector<FilePath> oldPaths = m_list->getPathsAsDisplayed();
	m_list->setPaths(utility::unique(utility::concat(oldPaths, paths)));
}


QtProjectWizzardContentPathsSource::QtProjectWizzardContentPathsSource(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES)
{
	m_showFilesString = "show files";

	setTitleString("Files & Directories to Index");
	setHelpString(
		"These paths define the files and directories that will be indexed by Sourcetrail. Provide a directory to recursively "
		"add all contained source and header files.<br />"
		"<br />"
		"If your project's source code resides in one location, but generated source files are kept at a different location, "
		"you will also need to add that directory.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizzardContentPathsSource::load()
{
	if (std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(m_settings)) // FIXME: pass msettings as required type
	{
		m_list->setPaths(pathSettings->getSourcePaths());
	}
}

void QtProjectWizzardContentPathsSource::save()
{
	if (std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(m_settings)) // FIXME: pass msettings as required type
	{
		pathSettings->setSourcePaths(m_list->getPathsAsDisplayed());
	}
}

std::vector<FilePath> QtProjectWizzardContentPathsSource::getFilePaths() const
{
	std::set<FilePath> allSourceFilePaths;
	if (std::shared_ptr<SourceGroupSettingsCxx> settings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings))
	{
		allSourceFilePaths = SourceGroupCxxEmpty(settings).getAllSourceFilePaths();
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaEmpty> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaEmpty>(m_settings))
	{
		allSourceFilePaths = SourceGroupJavaEmpty(settings).getAllSourceFilePaths();
	}

	std::vector<FilePath> filePaths;

	const FilePath projectPath = m_settings->getProjectDirectoryPath();
	for (FilePath path : allSourceFilePaths)
	{
		if (projectPath.exists())
		{
			path.makeRelativeTo(projectPath);
		}
		filePaths.push_back(path);
	}

	return filePaths;
}

QString QtProjectWizzardContentPathsSource::getFileNamesTitle() const
{
	return "Indexed Files";
}

QString QtProjectWizzardContentPathsSource::getFileNamesDescription() const
{
	return " files will be indexed.";
}

std::vector<FilePath> QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromSonargraphProject(
	std::shared_ptr<const SourceGroupSettingsCxxSonargraph> settings)
{
	std::set<FilePath> indexedHeaderPaths;
	{
		const FilePath sonargraphProjectPath = settings->getSonargraphProjectPathExpandedAndAbsolute();
		if (!sonargraphProjectPath.empty() && sonargraphProjectPath.exists())
		{
			if (std::shared_ptr<Sonargraph::Project> sonargraphProject = Sonargraph::Project::load(
				sonargraphProjectPath, settings->getLanguage()
			))
			{
				for (const FilePath& path : sonargraphProject->getAllSourceFilePathsCanonical())
				{
					indexedHeaderPaths.insert(path.getCanonical().getParentDirectory());
				}
				utility::append(indexedHeaderPaths, sonargraphProject->getAllCxxHeaderSearchPathsCanonical());
			}
		}
		else
		{
			LOG_WARNING("Unable to fetch indexed header paths. The provided Sonargraph project path does not exist.");
		}
	}
	return utility::getTopLevelPaths(indexedHeaderPaths);
}

std::vector<FilePath> QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromCodeblocksProject(
	std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> settings)
{
	const FilePath projectPath = settings->getProjectDirectoryPath();
	std::set<FilePath> indexedHeaderPaths;
	{
		const FilePath codeblocksProjectPath = settings->getCodeblocksProjectPathExpandedAndAbsolute();
		if (!codeblocksProjectPath.empty() && codeblocksProjectPath.exists())
		{
			if (std::shared_ptr<Codeblocks::Project> codeblocksProject = Codeblocks::Project::load(codeblocksProjectPath))
			{
				OrderedCache<FilePath, FilePath> canonicalDirectoryPathCache([](const FilePath& path) {
					return path.getCanonical();
				});

				for (const FilePath& path : codeblocksProject->getAllSourceFilePaths(settings))
				{
					indexedHeaderPaths.insert(canonicalDirectoryPathCache.getValue(path.getParentDirectory()));
				}
				utility::append(indexedHeaderPaths, codeblocksProject->getAllCxxHeaderSearchPathsCanonical());
			}
		}
		else
		{
			LOG_WARNING("Unable to fetch indexed header paths. The provided Sonargraph project path does not exist.");
		}
	}
	return utility::getTopLevelPaths(indexedHeaderPaths);
}

std::vector<FilePath> QtProjectWizzardContentIndexedHeaderPaths::getIndexedPathsDerivedFromCDB(
	std::shared_ptr<const SourceGroupSettingsCxxCdb> settings)
{
	std::set<FilePath> indexedHeaderPaths;
	{
		const FilePath cdbPath = settings->getCompilationDatabasePathExpandedAndAbsolute();
		if (!cdbPath.empty() && cdbPath.exists())
		{
			for (const FilePath& path : IndexerCommandCxxCdb::getSourceFilesFromCDB(cdbPath))
			{
				indexedHeaderPaths.insert(path.getCanonical().getParentDirectory());
			}
			for (const FilePath& path : utility::CompilationDatabase(cdbPath).getAllHeaderPaths())
			{
				if (path.exists())
				{
					indexedHeaderPaths.insert(path.getCanonical());
				}
			}
		}
		else
		{
			LOG_WARNING("Unable to fetch indexed header paths. The provided Compilation Database path does not exist.");
		}
	}

	return utility::getTopLevelPaths(indexedHeaderPaths);
}

QtProjectWizzardContentIndexedHeaderPaths::QtProjectWizzardContentIndexedHeaderPaths(
	const std::string& projectKindName, std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES)
	, m_projectKindName(projectKindName)
{
	m_showFilesString = "";

	setTitleString("Header Files & Directories to Index");
	setHelpString(QString::fromStdString(
		"The provided " + m_projectKindName + " already specifies which source files are part of your project. But Sourcetrail still "
		"needs to know which header files to index as part of your project and which to skip. Choosing to skip indexing "
		"your system headers or external frameworks will significantly improve the overall indexing performance.<br />"
		"<br />"
		"Use this list to define which header files should be indexed by Sourcetrail. Provide a directory to recursively "
		"add all contained files.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.<br />"
		"<br />"
		"<b>Hint</b>: Just enter the root path of your project if you want Sourcetrail to index all contained headers it "
		"encounters.<br />"
	));
}

void QtProjectWizzardContentIndexedHeaderPaths::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPaths::populate(layout, row);

	QPushButton* button = new QPushButton(QString::fromStdString("Select from " + m_projectKindName));
	button->setObjectName("windowButton");
	connect(button, &QPushButton::clicked, this, &QtProjectWizzardContentIndexedHeaderPaths::buttonClicked);

	layout->addWidget(button, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignRight | Qt::AlignTop);
	row++;
}

void QtProjectWizzardContentIndexedHeaderPaths::load()
{
	if (std::shared_ptr<SourceGroupSettingsWithIndexedHeaderPaths> cdbSettings = 
		std::dynamic_pointer_cast<SourceGroupSettingsWithIndexedHeaderPaths>(m_settings))
	{
		m_list->setPaths(cdbSettings->getIndexedHeaderPaths());
	}
}

void QtProjectWizzardContentIndexedHeaderPaths::save()
{
	if (std::shared_ptr<SourceGroupSettingsWithIndexedHeaderPaths> cdbSettings = 
		std::dynamic_pointer_cast<SourceGroupSettingsWithIndexedHeaderPaths>(m_settings))
	{
		cdbSettings->setIndexedHeaderPaths(m_list->getPathsAsDisplayed());
	}
}

bool QtProjectWizzardContentIndexedHeaderPaths::check()
{
	if (m_list->getPathsAsDisplayed().empty())
	{
		QMessageBox msgBox;
		msgBox.setText("You didn't specify any Header Files & Directories to Index.");
		msgBox.setInformativeText(QString::fromStdString(
			"Sourcetrail will only index the source files listed in the " + m_projectKindName + 
			" file and none of the included header files."
		));
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		return msgBox.exec() == QMessageBox::Ok;
	}
	else
	{
		return QtProjectWizzardContentPaths::check();
	}
}

void QtProjectWizzardContentIndexedHeaderPaths::buttonClicked()
{
	save();

	if (!m_filesDialog)
	{
		if (std::shared_ptr<SourceGroupSettingsCxxSonargraph> sonargraphSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxSonargraph>(m_settings))
		{
			const FilePath sonargraphProjectPath = sonargraphSettings->getSonargraphProjectPathExpandedAndAbsolute();
			if (!sonargraphProjectPath.exists())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided Sonargraph project path does not exist.");
				msgBox.setDetailedText(QString::fromStdWString(sonargraphProjectPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = std::make_shared<QtSelectPathsDialog>(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Sonargraph project. Red paths do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.");
			m_filesDialog->setup();

			connect(m_filesDialog.get(), &QtSelectPathsDialog::finished, this, &QtProjectWizzardContentIndexedHeaderPaths::savedFilesDialog);
			connect(m_filesDialog.get(), &QtSelectPathsDialog::canceled, this, &QtProjectWizzardContentIndexedHeaderPaths::closedFilesDialog);

			const FilePath projectPath = sonargraphSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(
				utility::convert<FilePath, FilePath>(
					getIndexedPathsDerivedFromSonargraphProject(sonargraphSettings),
					[&](const FilePath& path) { return path.getRelativeTo(projectPath); }
				),
				sonargraphSettings->getIndexedHeaderPaths(),
				m_settings->getProjectDirectoryPath()
			);
		}
		else if (std::shared_ptr<SourceGroupSettingsCxxCodeblocks> codeblocksSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(m_settings))
		{
			const FilePath codeblocksProjectPath = codeblocksSettings->getCodeblocksProjectPathExpandedAndAbsolute();
			if (!codeblocksProjectPath.exists())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided Code::Blocks project path does not exist.");
				msgBox.setDetailedText(QString::fromStdWString(codeblocksProjectPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = std::make_shared<QtSelectPathsDialog>(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Code::Blocks project. Red paths do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.");
			m_filesDialog->setup();

			connect(m_filesDialog.get(), &QtSelectPathsDialog::finished, this, &QtProjectWizzardContentIndexedHeaderPaths::savedFilesDialog);
			connect(m_filesDialog.get(), &QtSelectPathsDialog::canceled, this, &QtProjectWizzardContentIndexedHeaderPaths::closedFilesDialog);

			const FilePath projectPath = codeblocksSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(
				utility::convert<FilePath, FilePath>(
					getIndexedPathsDerivedFromCodeblocksProject(codeblocksSettings),
					[&](const FilePath& path) { return path.getRelativeTo(projectPath); }
				),
				codeblocksSettings->getIndexedHeaderPaths(),
				m_settings->getProjectDirectoryPath()
			);
		}
		else if (std::shared_ptr<SourceGroupSettingsCxxCdb> cdbSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
		{
			const FilePath cdbPath = cdbSettings->getCompilationDatabasePathExpandedAndAbsolute();
			if (!cdbPath.exists())
			{
				QMessageBox msgBox;
				msgBox.setText("The provided Compilation Database path does not exist.");
				msgBox.setDetailedText(QString::fromStdWString(cdbPath.wstr()));
				msgBox.exec();
				return;
			}

			m_filesDialog = std::make_shared<QtSelectPathsDialog>(
				"Select from Include Paths",
				"The list contains all Include Paths found in the Compilation Database. Red paths do not exist. Select the "
				"paths containing the header files you want to index with Sourcetrail.");
			m_filesDialog->setup();

			connect(m_filesDialog.get(), &QtSelectPathsDialog::finished, this, &QtProjectWizzardContentIndexedHeaderPaths::savedFilesDialog);
			connect(m_filesDialog.get(), &QtSelectPathsDialog::canceled, this, &QtProjectWizzardContentIndexedHeaderPaths::closedFilesDialog);

			const FilePath projectPath = cdbSettings->getProjectDirectoryPath();

			dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(
				utility::convert<FilePath, FilePath>(
					getIndexedPathsDerivedFromCDB(cdbSettings),
					[&](const FilePath& path) { return path.getRelativeTo(projectPath); }
				),
				cdbSettings->getIndexedHeaderPaths(),
				m_settings->getProjectDirectoryPath()
			);
		}
	}

	if (m_filesDialog)
	{
		m_filesDialog->showWindow();
		m_filesDialog->raise();
	}
}

void QtProjectWizzardContentIndexedHeaderPaths::savedFilesDialog()
{
	m_list->setPaths(dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->getPathsList());
	closedFilesDialog();
}


QtProjectWizzardContentPathsExclude::QtProjectWizzardContentPathsExclude(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES, false)
{
	setTitleString("Excluded Files & Directories");
	setHelpString(
		"<p>These paths define the files and directories that will be left out from indexing.</p>"
		"<p>Hints:"
		"<ul>"
		"<li>You can use the wildcard \"*\" which represents characters except \"\\\" or \"/\" (e.g. \"src/*/test.h\" matches \"src/app/test.h\" but does not match \"src/app/widget/test.h\" or \"src/test.h\")</li>"
		"<li>You can use the wildcard \"**\" which represents arbitrary characters (e.g. \"src**test.h\" matches \"src/app/test.h\" as well as \"src/app/widget/test.h\" or \"src/test.h\")</li>"
		"<li>You can make use of environment variables with ${ENV_VAR}</li>"
		"</ul></p>"
	);
}

void QtProjectWizzardContentPathsExclude::load()
{
	if (std::shared_ptr<SourceGroupSettingsWithExcludeFilters> settings = std::dynamic_pointer_cast<SourceGroupSettingsWithExcludeFilters>(m_settings)) // FIXME: pass msettings as required type
	{
		m_list->setPaths(utility::convert<std::wstring, FilePath>(settings->getExcludeFilterStrings(), [](const std::wstring& s) { return FilePath(s); }));
	}
}

void QtProjectWizzardContentPathsExclude::save()
{
	if (std::shared_ptr<SourceGroupSettingsWithExcludeFilters> settings = std::dynamic_pointer_cast<SourceGroupSettingsWithExcludeFilters>(m_settings)) // FIXME: pass msettings as required type
	{
		settings->setExcludeFilterStrings(utility::toWStrings(m_list->getPathsAsDisplayed()));
	}
}


QtProjectWizzardContentPathsHeaderSearch::QtProjectWizzardContentPathsHeaderSearch(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool indicateAsAdditional
)
	: QtProjectWizzardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY)
	, m_showDetectedIncludesResultFunctor(std::bind(
		&QtProjectWizzardContentPathsHeaderSearch::showDetectedIncludesResult, this, std::placeholders::_1))
	, m_showValidationResultFunctor(std::bind(
		&QtProjectWizzardContentPathsHeaderSearch::showValidationResult, this, std::placeholders::_1))
	, m_indicateAsAdditional(indicateAsAdditional)
{
	setTitleString(m_indicateAsAdditional ? "Additional Include Paths" : "Include Paths");
	setHelpString(
		(
			(m_indicateAsAdditional ? "<b>Note</b>: Use the Additional Include Paths to add paths that are missing in the "
				"referenced project file.<br /><br />" : ""
			) + std::string(
				"Include Paths are used for resolving #include directives in the indexed source and header files. These paths are "
				"usually passed to the compiler with the '-I' or '-iquote' flags.<br />"
				"<br />"
				"Add all paths #include directives throughout your project are relative to. If all #include directives are "
				"specified relative to the project's root directory, please add that root directory here.<br />"
				"<br />"
				"If your project also includes files from external libraries (e.g. boost), please add these directories as well "
				"(e.g. add 'path/to/boost_home/include').<br />"
				"<br />"
				"You can make use of environment variables with ${ENV_VAR}."
			)
		).c_str()
	);
}

void QtProjectWizzardContentPathsHeaderSearch::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPaths::populate(layout, row);

	if (!m_indicateAsAdditional)
	{
		{
			QPushButton* detectionButton = new QPushButton("auto-detect");
			detectionButton->setObjectName("windowButton");
			connect(detectionButton, &QPushButton::clicked, this, &QtProjectWizzardContentPathsHeaderSearch::detectIncludesButtonClicked);
			layout->addWidget(detectionButton, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
		}
		{
			QPushButton* validateionButton = new QPushButton("validate include directives");
			validateionButton->setObjectName("windowButton");
			connect(validateionButton, &QPushButton::clicked, this, &QtProjectWizzardContentPathsHeaderSearch::validateIncludesButtonClicked);
			layout->addWidget(validateionButton, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignRight | Qt::AlignTop);
		}
		row++;
	}
}

void QtProjectWizzardContentPathsHeaderSearch::load()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		m_list->setPaths(cxxSettings->getHeaderSearchPaths());
	}
}

void QtProjectWizzardContentPathsHeaderSearch::save()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setHeaderSearchPaths(m_list->getPathsAsDisplayed());
	}
}

bool QtProjectWizzardContentPathsHeaderSearch::isScrollAble() const
{
	return true;
}

void QtProjectWizzardContentPathsHeaderSearch::detectIncludesButtonClicked()
{
	m_window->saveContent();

	m_pathsDialog = std::make_shared<QtPathListDialog>(
		"Detect Include Paths",
		"<p>Automatically search for header files in the paths provided below to find missing include paths for "
		"unresolved include directives in your source code.</p>"
		"<p>The \"Files & Directories to Index\" will be searched by default, but you can add further paths, such "
		"as directories of third-party libraries, if required.</p>",
		QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY
	);

	m_pathsDialog->setup();
	m_pathsDialog->updateNextButton("Start");
	m_pathsDialog->setCloseVisible(true);

	m_pathsDialog->setRelativeRootDirectory(m_settings->getProjectDirectoryPath());
	if (std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(m_settings)) // FIXME: pass msettings as required type
	{
		m_pathsDialog->setPaths(pathSettings->getSourcePaths(), true);
	}
	m_pathsDialog->showWindow();

	connect(m_pathsDialog.get(), &QtPathListDialog::finished, this, &QtProjectWizzardContentPathsHeaderSearch::finishedSelectDetectIncludesRootPathsDialog);
	connect(m_pathsDialog.get(), &QtPathListDialog::canceled, this, &QtProjectWizzardContentPathsHeaderSearch::closedPathsDialog);
}

void QtProjectWizzardContentPathsHeaderSearch::validateIncludesButtonClicked()
{
	// TODO: regard Force Includes here, too!
	m_window->saveContent();
	
	std::thread([&]()
	{
		std::shared_ptr<SourceGroupSettingsWithSourceExtensions> extensionSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourceExtensions>(m_settings);
		std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(m_settings);
		std::shared_ptr<SourceGroupSettingsWithExcludeFilters> excludeFilterSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithExcludeFilters>(m_settings);

		if (extensionSettings && pathSettings && excludeFilterSettings) // FIXME: pass msettings as required type
		{
			std::vector<IncludeDirective> unresolvedIncludes;
			{
				std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

				std::set<FilePath> sourceFilePaths;
				std::vector<FilePath> indexedFilePaths;
				std::vector<FilePath> headerSearchPaths;

				{
					std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
					dialogView->showUnknownProgressDialog(L"Processing", L"Gathering Source Files");
					ScopedFunctor dialogHider([&dialogView](){
						dialogView->hideUnknownProgressDialog();
					});

					FileManager fileManager;
					fileManager.update(
						pathSettings->getSourcePathsExpandedAndAbsolute(),
						excludeFilterSettings->getExcludeFiltersExpandedAndAbsolute(),
						extensionSettings->getSourceExtensions()
					);
					sourceFilePaths = fileManager.getAllSourceFilePaths();

					indexedFilePaths = pathSettings->getSourcePathsExpandedAndAbsolute();

					headerSearchPaths = ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded();
					if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings =
							std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings))
					{
						utility::append(headerSearchPaths, cxxSettings->getHeaderSearchPathsExpandedAndAbsolute());
					}
				}
				{
					std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
					ScopedFunctor dialogHider([&dialogView](){
						dialogView->hideProgressDialog();
					});

					unresolvedIncludes = IncludeProcessing::getUnresolvedIncludeDirectives(
						sourceFilePaths,
						utility::toSet(indexedFilePaths),
						utility::toSet(headerSearchPaths),
						log2(sourceFilePaths.size()),
						[&](const float progress)
						{
							Application::getInstance()->getDialogView()->showProgressDialog(
								L"Processing", std::to_wstring(int(progress * sourceFilePaths.size())) + L" Files", int(progress * 100.0f)
							);
						}
					);
				}
			}
			m_showValidationResultFunctor(unresolvedIncludes);
		}
	}).detach();
}


void QtProjectWizzardContentPathsHeaderSearch::finishedSelectDetectIncludesRootPathsDialog()
{
	// TODO: regard Force Includes here, too!
	const std::vector<FilePath> searchedPaths = m_settings->makePathsExpandedAndAbsolute(m_pathsDialog->getPaths());
	closedPathsDialog();
	
	std::thread([=]()
	{
		std::shared_ptr<SourceGroupSettingsWithSourceExtensions> extensionSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourceExtensions>(m_settings);
		std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(m_settings);
		std::shared_ptr<SourceGroupSettingsWithExcludeFilters> excludeFilterSettings = std::dynamic_pointer_cast<SourceGroupSettingsWithExcludeFilters>(m_settings);

		if (extensionSettings && pathSettings && excludeFilterSettings) // FIXME: pass msettings as required type
		{
			std::set<FilePath> detectedHeaderSearchPaths;
			{
				std::shared_ptr<QtDialogView> dialogView = std::dynamic_pointer_cast<QtDialogView>(Application::getInstance()->getDialogView());

				std::set<FilePath> sourceFilePaths;
				std::vector<FilePath> headerSearchPaths;
				{
					dialogView->setParentWindow(m_window);
					dialogView->showUnknownProgressDialog(L"Processing", L"Gathering Source Files");
					ScopedFunctor dialogHider([&dialogView]() {
						dialogView->hideUnknownProgressDialog();
					});

					FileManager fileManager;
					fileManager.update(
						pathSettings->getSourcePathsExpandedAndAbsolute(),
						excludeFilterSettings->getExcludeFiltersExpandedAndAbsolute(),
						extensionSettings->getSourceExtensions()
					);
					sourceFilePaths = fileManager.getAllSourceFilePaths();

					headerSearchPaths = ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded();
					if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings =
						std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings))
					{
						utility::append(headerSearchPaths, cxxSettings->getHeaderSearchPathsExpandedAndAbsolute());
					}
				}
				{
					dialogView->setParentWindow(m_window);
					ScopedFunctor dialogHider([&dialogView]() {
						dialogView->hideProgressDialog();
					});

					detectedHeaderSearchPaths = IncludeProcessing::getHeaderSearchDirectories(
						sourceFilePaths,
						utility::toSet(searchedPaths),
						utility::toSet(headerSearchPaths),
						log2(sourceFilePaths.size()),
						[&](const float progress)
						{
							Application::getInstance()->getDialogView()->showProgressDialog(
								L"Processing", std::to_wstring(int(progress * sourceFilePaths.size())) + L" Files", int(progress * 100.0f)
							);
						}
					);
				}
			}

			m_showDetectedIncludesResultFunctor(detectedHeaderSearchPaths);
		}
	}).detach();
}

void QtProjectWizzardContentPathsHeaderSearch::finishedAcceptDetectedIncludePathsDialog()
{
	const std::vector<std::wstring> detectedPaths = utility::split<std::vector<std::wstring>>(m_filesDialog->getText(), L"\n");
	closedFilesDialog();

	std::vector<FilePath> headerSearchPaths = m_list->getPathsAsDisplayed();

	headerSearchPaths.reserve(headerSearchPaths.size() + detectedPaths.size());
	for (const std::wstring& detectedPath : detectedPaths)
	{
		if (!detectedPath.empty())
		{
			headerSearchPaths.push_back(FilePath(detectedPath));
		}
	}

	m_list->setPaths(headerSearchPaths);
}

void QtProjectWizzardContentPathsHeaderSearch::closedPathsDialog()
{
	m_pathsDialog->hide();
	m_pathsDialog.reset();

	window()->raise();
}

void QtProjectWizzardContentPathsHeaderSearch::showDetectedIncludesResult(const std::set<FilePath>& detectedHeaderSearchPaths)
{
	const std::set<FilePath> headerSearchPaths = utility::toSet(m_settings->makePathsExpandedAndAbsolute(m_list->getPathsAsDisplayed()));

	std::vector<FilePath> additionalHeaderSearchPaths;
	for (const FilePath& detectedHeaderSearchPath : detectedHeaderSearchPaths)
	{
		if (headerSearchPaths.find(detectedHeaderSearchPath) == headerSearchPaths.end())
		{
			additionalHeaderSearchPaths.push_back(detectedHeaderSearchPath);
		}
	}

	if (additionalHeaderSearchPaths.empty())
	{
		QMessageBox msgBox;
		msgBox.setText("<p>No additional include paths have been detected while searching the provided paths.</p>");
		msgBox.exec();
	}
	else
	{
		std::wstring detailedText = L"";
		const FilePath relativeRoot = m_list->getRelativeRootDirectory();
		for (const FilePath& path : additionalHeaderSearchPaths)
		{
			if (!relativeRoot.empty())
			{
				const FilePath relPath = path.getRelativeTo(relativeRoot);
				if (relPath.wstr().size() < path.wstr().size())
				{
					detailedText += relPath.wstr() + L"\n";
				}
				else
				{
					detailedText += path.wstr() + L"\n";
				}
			}
		}

		m_filesDialog = std::make_shared<QtTextEditDialog>(
			"Detected Include Paths",
			(
				"<p>The following <b>" + std::to_string(additionalHeaderSearchPaths.size()) + "</b> include paths have been "
				"detected and will be added to the include paths of this Source Group.<b>"
			).c_str()
		);

		m_filesDialog->setup();
		m_filesDialog->setReadOnly(true);
		m_filesDialog->setCloseVisible(true);
		m_filesDialog->updateNextButton("Add");

		m_filesDialog->setText(detailedText);
		m_filesDialog->showWindow();

		connect(m_filesDialog.get(), &QtTextEditDialog::finished, this, &QtProjectWizzardContentPathsHeaderSearch::finishedAcceptDetectedIncludePathsDialog);
		connect(m_filesDialog.get(), &QtTextEditDialog::canceled, this, &QtProjectWizzardContentPathsHeaderSearch::closedFilesDialog);
	}
}

void QtProjectWizzardContentPathsHeaderSearch::showValidationResult(const std::vector<IncludeDirective>& unresolvedIncludes)
{
	if (unresolvedIncludes.empty())
	{
		QMessageBox msgBox;
		msgBox.setText("<p>All include directives throughout the indexed files have been resolved.</p>");
		msgBox.exec();
	}
	else
	{
		std::map<std::wstring, std::map<size_t, std::wstring>> orderedIncludes;
		for (const IncludeDirective& unresolvedInclude: unresolvedIncludes)
		{
			orderedIncludes[unresolvedInclude.getIncludingFile().wstr()].emplace(
				unresolvedInclude.getLineNumber(), unresolvedInclude.getDirective());
		}

		std::wstring detailedText = L"";
		for (const auto& p: orderedIncludes)
		{
			detailedText += p.first + L"\n";

			for (const auto& p2: p.second)
			{
				detailedText += std::to_wstring(p2.first) + L":\t" + p2.second + L"\n";
			}

			detailedText += L"\n";
		}

		m_filesDialog = std::make_shared<QtTextEditDialog>("Unresolved Include Directives",
			("<p>The indexed files contain <b>" + std::to_string(unresolvedIncludes.size()) + "</b> include directive" +
			(unresolvedIncludes.size() == 1 ? "" : "s") + " that could not be resolved correctly. Please check the details "
			"and add the respective header search paths.</p>"
			"<p><b>Note</b>: This is only a quick pass that does not regard block commenting or conditional preprocessor "
			"directives. This means that some of the unresolved includes may actually not be required by the indexer.</p>"
		).c_str());

		m_filesDialog->setup();
		m_filesDialog->setCloseVisible(false);
		m_filesDialog->setReadOnly(true);

		m_filesDialog->setText(detailedText);
		m_filesDialog->showWindow();

		connect(m_filesDialog.get(), &QtTextEditDialog::finished, this, &QtProjectWizzardContentPathsHeaderSearch::closedFilesDialog);
		connect(m_filesDialog.get(), &QtTextEditDialog::canceled, this, &QtProjectWizzardContentPathsHeaderSearch::closedFilesDialog);
	}
}

QtProjectWizzardContentPathsHeaderSearchGlobal::QtProjectWizzardContentPathsHeaderSearchGlobal(
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(std::shared_ptr<SourceGroupSettings>(), window, QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY)
{
	setTitleString("Global Include Paths");
	setHelpString(
		"The Global Include Paths will be used in all your projects in addition to the project specific Include Paths. "
		"These paths are usually passed to the compiler with the '-isystem' flag.<br />"
		"<br />"
		"Use them to add system header paths (See <a href=\"https://sourcetrail.com/documentation/#FindingSystemHeaderLocations\">"
		"Finding System Header Locations</a> or use the auto detection below)."
	);

	m_pathDetector = utility::getCxxHeaderPathDetector();
	m_makePathsRelativeToProjectFileLocation = false;
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::load()
{
	m_list->setPaths(ApplicationSettings::getInstance()->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::save()
{
	ApplicationSettings::getInstance()->setHeaderSearchPaths(m_list->getPathsAsDisplayed());
	ApplicationSettings::getInstance()->save();
}


QtProjectWizzardContentPathsFrameworkSearch::QtProjectWizzardContentPathsFrameworkSearch(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool indicateAsAdditional
)
	: QtProjectWizzardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY)
{
	setTitleString(indicateAsAdditional ? "Additional Framework Search Paths" : "Framework Search Paths");
	setHelpString(
		"Framework Search Paths define where MacOS framework containers (.framework), that your project depends on, are "
		"found. These paths are usually passed to the compiler with the '-iframework' flag.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizzardContentPathsFrameworkSearch::load()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		m_list->setPaths(cxxSettings->getFrameworkSearchPaths());
	}
}

void QtProjectWizzardContentPathsFrameworkSearch::save()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setFrameworkSearchPaths(m_list->getPathsAsDisplayed());
	}
}

bool QtProjectWizzardContentPathsFrameworkSearch::isScrollAble() const
{
	return true;
}

QtProjectWizzardContentPathsFrameworkSearchGlobal::QtProjectWizzardContentPathsFrameworkSearchGlobal(
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(std::shared_ptr<SourceGroupSettings>(), window, QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY)
{
	setTitleString("Global Framework Search Paths");
	setHelpString(
		"The Global Framework Search Paths will be used in all your projects - in addition to the project specific "
		"Framework Search Paths.<br />"
		"<br />"
		"They define where MacOS framework containers (.framework) are found "
		"(See <a href=\"https://sourcetrail.com/documentation/#FindingSystemHeaderLocations\">"
		"Finding System Header Locations</a> or use the auto detection below)."
	);

	m_pathDetector = utility::getCxxFrameworkPathDetector();
	m_makePathsRelativeToProjectFileLocation = false;
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::load()
{
	m_list->setPaths(ApplicationSettings::getInstance()->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::save()
{
	ApplicationSettings::getInstance()->setFrameworkSearchPaths(m_list->getPathsAsDisplayed());
	ApplicationSettings::getInstance()->save();
}


QtProjectWizzardContentPathsClassJava::QtProjectWizzardContentPathsClassJava(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES)
{
	setTitleString("Class Path");
	setHelpString(
		"Enter all the .jar files your project depends on. If your project depends on uncompiled java code that should "
		"not be indexed, please add the root directory of those .java files here (the one where all the package names "
		"are relative to).<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizzardContentPathsClassJava::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPaths::populate(layout, row);

	QLabel* label = createFormLabel("JRE System Library");
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	m_useJreSystemLibraryCheckBox = new QCheckBox("Use JRE System Library", this);

	layout->addWidget(m_useJreSystemLibraryCheckBox, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentPathsClassJava::load()
{
	std::shared_ptr<SourceGroupSettingsWithClasspath> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithClasspath>(m_settings);
	if (settings)
	{
		m_list->setPaths(settings->getClasspath());
		m_useJreSystemLibraryCheckBox->setChecked(settings->getUseJreSystemLibrary());
	}
}

void QtProjectWizzardContentPathsClassJava::save()
{
	std::shared_ptr<SourceGroupSettingsWithClasspath> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithClasspath>(m_settings);
	if (settings)
	{
		settings->setClasspath(m_list->getPathsAsDisplayed());
		settings->setUseJreSystemLibrary(m_useJreSystemLibraryCheckBox->isChecked());
	}
}
