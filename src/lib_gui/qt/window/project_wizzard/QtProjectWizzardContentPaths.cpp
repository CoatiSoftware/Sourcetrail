#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

#include "Application.h"
#include "component/view/DialogView.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "utility/IncludeDirective.h"
#include "utility/IncludeProcessing.h"
#include "qt/element/QtDirectoryListBox.h"
#include "qt/view/QtDialogView.h"
#include "qt/window/QtPathListDialog.h"
#include "qt/window/QtSelectPathsDialog.h"
#include "settings/ApplicationSettings.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsJava.h"
#include "utility/CompilationDatabase.h"
#include "utility/file/FileManager.h"
#include "utility/ScopedFunctor.h"
#include "utility/utility.h"
#include "utility/utilityFile.h"
#include "utility/utilityPathDetection.h"
#include "utility/utilityString.h"

QtProjectWizzardContentPaths::QtProjectWizzardContentPaths(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_makePathsRelativeToProjectFileLocation(true)
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

	m_list = new QtDirectoryListBox(this, m_titleString);

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
	QString missingPaths;
	std::vector<FilePath> existingPaths;

	for (const FilePath& path : m_list->getList())
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
		msgBox.setText(QString("Some provided paths do not exist at \"%1\". Do you want to remove them "
			"before continuing?").arg(m_titleString));
		msgBox.setDetailedText(missingPaths);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		int ret = msgBox.exec();

		if (ret == QMessageBox::Yes)
		{
			m_list->setList(existingPaths);
			save();
		}
		else if (ret == QMessageBox::Cancel)
		{
			return false;
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
	std::vector<FilePath> oldPaths = m_list->getList();
	m_list->setList(utility::unique(utility::concat(oldPaths, paths)));
}


QtProjectWizzardContentPathsSource::QtProjectWizzardContentPathsSource(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
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
	m_list->setList(m_settings->getSourcePaths());
}

void QtProjectWizzardContentPathsSource::save()
{
	m_settings->setSourcePaths(m_list->getList());
}

std::vector<std::string> QtProjectWizzardContentPathsSource::getFileNames() const
{
	std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

	ScopedFunctor scopedFunctor([&dialogView](){
		dialogView->hideUnknownProgressDialog();
	});

	std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
	dialogView->showUnknownProgressDialog("Processing", "Gathering Source Files");

	FileManager fileManager;
	fileManager.update(
		m_settings->getSourcePathsExpandedAndAbsolute(),
		m_settings->getExcludePathsExpandedAndAbsolute(),
		m_settings->getSourceExtensions()
	);

	const std::set<FilePath> filePaths = fileManager.getAllSourceFilePathsRelative(m_settings->getProjectDirectoryPath());

	std::vector<std::string> list;
	list.resize(filePaths.size());
	std::transform(filePaths.begin(), filePaths.end(), list.begin(), [](const FilePath& p){ return p.str(); });

	return list;
}

QString QtProjectWizzardContentPathsSource::getFileNamesTitle() const
{
	return "Indexed Files";
}

QString QtProjectWizzardContentPathsSource::getFileNamesDescription() const
{
	return " files will be indexed.";
}


std::vector<FilePath> QtProjectWizzardContentPathsCDBHeader::getTopLevelHeaderSearchPaths(
	std::shared_ptr<SourceGroupSettingsCxxCdb> settings)
{
	const FilePath cdbPath = settings->getCompilationDatabasePathExpandedAndAbsolute();
	if (!cdbPath.exists())
	{
		LOG_WARNING("Unable to fetch top level header search directories. The provided Compilation Database path does not exist.");
		return std::vector<FilePath>();
	}
	const std::vector<FilePath> sourcePaths = settings->getSourcePaths();
	return utility::getTopLevelPaths(utility::unique(utility::concat(
		sourcePaths, utility::CompilationDatabase(cdbPath).getAllHeaderPaths()
	)));
}

QtProjectWizzardContentPathsCDBHeader::QtProjectWizzardContentPathsCDBHeader(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPathsSource(settings, window)
{
	m_showFilesString = "";

	setTitleString("Header Files & Directories to Index");
	setHelpString(
		"Your Compilation Database already specifies which source files are part of your project. But Sourcetrail still "
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
	);
}

void QtProjectWizzardContentPathsCDBHeader::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPaths::populate(layout, row);

	QPushButton* button = new QPushButton("Select from Include Paths");
	button->setObjectName("windowButton");
	connect(button, &QPushButton::clicked, this, &QtProjectWizzardContentPathsCDBHeader::buttonClicked);

	layout->addWidget(button, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignRight | Qt::AlignTop);
	row++;
}

void QtProjectWizzardContentPathsCDBHeader::load()
{
	if (m_settings->getSourcePaths().empty())
	{
		std::shared_ptr<SourceGroupSettingsCxxCdb> cdbSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings);
		std::set<FilePath> sourcePaths;

		const FilePath projectPath = m_settings->getProjectDirectoryPath();
		const FilePath cdbPath = cdbSettings->getCompilationDatabasePathExpandedAndAbsolute();

		if (!cdbPath.empty() && cdbPath.exists())
		{
			for (const FilePath& path : IndexerCommandCxxCdb::getSourceFilesFromCDB(cdbPath))
			{
				sourcePaths.insert(path.getParentDirectory());
			}
		}

		for (const FilePath& path : getTopLevelHeaderSearchPaths(cdbSettings))
		{
			if (path.exists() && projectPath.contains(path))
			{
				sourcePaths.insert(path);
			}
		}

		std::vector<FilePath> rootPaths;

		FilePath lastPath;
		for (const FilePath& path : sourcePaths)
		{
			if (lastPath.empty() || !lastPath.contains(path)) // don't add subdirectories of already added paths
			{
				lastPath = path;
				rootPaths.push_back(path.getRelativeTo(projectPath));
			}
		}

		m_settings->setSourcePaths(rootPaths);
	}

	QtProjectWizzardContentPathsSource::load();
}

bool QtProjectWizzardContentPathsCDBHeader::check()
{
	if (!m_list->getList().size())
	{
		QMessageBox msgBox;
		msgBox.setText("You didn't specify any Header Files & Directories to Index.");
		msgBox.setInformativeText(
			"Sourcetrail will only index the source files listed in the compilation database file and none of the included "
			"header files.");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();

		return ret == QMessageBox::Ok;
	}
	else
	{
		return QtProjectWizzardContentPaths::check();
	}
}

void QtProjectWizzardContentPathsCDBHeader::buttonClicked()
{
	save();

	if (!m_filesDialog)
	{
		const FilePath cdbPath =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings)->getCompilationDatabasePathExpandedAndAbsolute();
		if (!cdbPath.exists())
		{
			QMessageBox msgBox;
			msgBox.setText("The provided Compilation Database path does not exist.");
			msgBox.setDetailedText(QString::fromStdString(cdbPath.str()));
			msgBox.exec();
			return;
		}

		m_filesDialog = std::make_shared<QtSelectPathsDialog>(
			"Select from Include Paths",
			"The list contains all Include Paths found in the Compilation Database. Red paths do not exist. Select the "
			"paths containing the header files you want to index with Sourcetrail.");
		m_filesDialog->setup();

		connect(m_filesDialog.get(), &QtSelectPathsDialog::finished, this, &QtProjectWizzardContentPathsCDBHeader::savedFilesDialog);
		connect(m_filesDialog.get(), &QtSelectPathsDialog::canceled, this, &QtProjectWizzardContentPathsCDBHeader::closedFilesDialog);

		dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->setPathsList(
			getTopLevelHeaderSearchPaths(std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings)),
			m_settings->getSourcePaths()
		);
	}

	m_filesDialog->showWindow();
	m_filesDialog->raise();
}

void QtProjectWizzardContentPathsCDBHeader::savedFilesDialog()
{
	// TODO: extend instead of replace
	m_list->setList(dynamic_cast<QtSelectPathsDialog*>(m_filesDialog.get())->getPathsList());

	closedFilesDialog();
}

QtProjectWizzardContentPathsExclude::QtProjectWizzardContentPathsExclude(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setTitleString("Excluded Files & Directories");
	setHelpString(
		"These paths define the files and directories that will be left out from indexing.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."
	);
}

void QtProjectWizzardContentPathsExclude::load()
{
	m_list->setList(m_settings->getExcludePaths());
}

void QtProjectWizzardContentPathsExclude::save()
{
	m_settings->setExcludePaths(m_list->getList());
}


QtProjectWizzardContentPathsHeaderSearch::QtProjectWizzardContentPathsHeaderSearch(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool isCDB
)
	: QtProjectWizzardContentPaths(settings, window)
	, m_showDetectedIncludesResultFunctor(std::bind(
		&QtProjectWizzardContentPathsHeaderSearch::showDetectedIncludesResult, this, std::placeholders::_1))
	, m_showValidationResultFunctor(std::bind(
		&QtProjectWizzardContentPathsHeaderSearch::showValidationResult, this, std::placeholders::_1))
	, m_isCdb(isCDB)
{
	setTitleString(m_isCdb ? "Additional Include Paths" : "Include Paths");
	setHelpString(
		((m_isCdb ? "<b>Note</b>: Use the Additional Include Paths to add paths that are missing in the Compilation "
			"Database.<br /><br />" : "") + std::string(
		"Include Paths are used for resolving #include directives in the indexed source and header files. These paths are "
		"usually passed to the compiler with the '-I' or '-iquote' flags.<br />"
		"<br />"
		"Add all paths #include directives throughout your project are relative to. If all #include directives are "
		"specified relative to the project's root directory, please add that root directory here.<br />"
		"<br />"
		"If your project also includes files from external libraries (e.g. boost), please add these directories as well "
		"(e.g. add 'path/to/boost_home/include').<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.")).c_str()
	);
}

void QtProjectWizzardContentPathsHeaderSearch::populate(QGridLayout* layout, int& row)
{
	QtProjectWizzardContentPaths::populate(layout, row);

	if (!m_isCdb)
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
		m_list->setList(cxxSettings->getHeaderSearchPaths());
	}
}

void QtProjectWizzardContentPathsHeaderSearch::save()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setHeaderSearchPaths(m_list->getList());
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
		"as directories of third-party libraries, if required.</p>"
	);

	m_pathsDialog->setup();
	m_pathsDialog->updateNextButton("Start");
	m_pathsDialog->setCloseVisible(true);

	m_pathsDialog->setRelativeRootDirectory(m_settings->getProjectDirectoryPath());
	m_pathsDialog->setPaths(m_settings->getSourcePaths(), true);
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
		std::vector<IncludeDirective> unresolvedIncludes;
		{
			std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

			std::set<FilePath> sourceFilePaths;
			std::vector<FilePath> indexedFilePaths;
			std::vector<FilePath> headerSearchPaths;

			{
				std::dynamic_pointer_cast<QtDialogView>(dialogView)->setParentWindow(m_window);
				dialogView->showUnknownProgressDialog("Processing", "Gathering Source Files");
				ScopedFunctor dialogHider([&dialogView](){
					dialogView->hideUnknownProgressDialog();
				});

				FileManager fileManager;
				fileManager.update(
					m_settings->getSourcePathsExpandedAndAbsolute(),
					m_settings->getExcludePathsExpandedAndAbsolute(),
					m_settings->getSourceExtensions()
				);
				sourceFilePaths = fileManager.getAllSourceFilePaths();

				headerSearchPaths = ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded();

				if (std::shared_ptr<SourceGroupSettingsCxx> cxxSettings =
						std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings))
				{
					indexedFilePaths = cxxSettings->getSourcePaths();
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
							"Processing", std::to_string(int(progress * sourceFilePaths.size())) + " Files", int(progress * 100.0f)
						);
					}
				);
			}
		}
		m_showValidationResultFunctor(unresolvedIncludes);
	}).detach();
}


void QtProjectWizzardContentPathsHeaderSearch::finishedSelectDetectIncludesRootPathsDialog()
{
	// TODO: regard Force Includes here, too!
	const std::vector<FilePath> searchedPaths = m_settings->makePathsExpandedAndAbsolute(m_pathsDialog->getPaths());
	closedPathsDialog();

	std::thread([=]()
	{
		std::set<FilePath> detectedHeaderSearchPaths;
		{
			std::shared_ptr<QtDialogView> dialogView = std::dynamic_pointer_cast<QtDialogView>(Application::getInstance()->getDialogView());

			std::set<FilePath> sourceFilePaths;
			std::vector<FilePath> headerSearchPaths;
			{
				dialogView->setParentWindow(m_window);
				dialogView->showUnknownProgressDialog("Processing", "Gathering Source Files");
				ScopedFunctor dialogHider([&dialogView]() {
					dialogView->hideUnknownProgressDialog();
				});

				FileManager fileManager;
				fileManager.update(
					m_settings->getSourcePathsExpandedAndAbsolute(),
					m_settings->getExcludePathsExpandedAndAbsolute(),
					m_settings->getSourceExtensions()
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
							"Processing", std::to_string(int(progress * sourceFilePaths.size())) + " Files", int(progress * 100.0f)
						);
					}
				);
			}
		}

		m_showDetectedIncludesResultFunctor(detectedHeaderSearchPaths);
	}).detach();
}

void QtProjectWizzardContentPathsHeaderSearch::finishedAcceptDetectedIncludePathsDialog()
{
	const std::vector<std::wstring> detectedPaths = utility::split<std::vector<std::wstring>>(m_filesDialog->getWText(), L"\n");
	closedFilesDialog();

	std::vector<std::wstring> headerSearchPaths = m_list->getWStringList();

	headerSearchPaths.reserve(headerSearchPaths.size() + detectedPaths.size());
	for (const std::wstring& detectedPath : detectedPaths)
	{
		if (!detectedPath.empty())
		{
			headerSearchPaths.push_back(detectedPath);
		}
	}

	m_list->setWStringList(headerSearchPaths);
}

void QtProjectWizzardContentPathsHeaderSearch::closedPathsDialog()
{
	m_pathsDialog->hide();
	m_pathsDialog.reset();

	window()->raise();
}

void QtProjectWizzardContentPathsHeaderSearch::showDetectedIncludesResult(const std::set<FilePath>& detectedHeaderSearchPaths)
{
	const std::set<FilePath> headerSearchPaths = utility::toSet(m_settings->makePathsExpandedAndAbsolute(m_list->getList()));

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
		std::string detailedText = "";
		FilePath relativeRoot = m_list->getRelativeRootDirectory();
		for (const FilePath& path : additionalHeaderSearchPaths)
		{
			if (!relativeRoot.empty())
			{
				const FilePath relPath = path.getRelativeTo(relativeRoot);
				if (relPath.str().size() < path.str().size())
				{
					detailedText += relPath.str() + "\n";
					continue;
				}
			}

			detailedText += path.str() + "\n";
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
		std::map<std::string, std::map<size_t, std::string>> orderedIncludes;
		for (const IncludeDirective& unresolvedInclude: unresolvedIncludes)
		{
			orderedIncludes[unresolvedInclude.getIncludingFile().str()].emplace(
				unresolvedInclude.getLineNumber(), unresolvedInclude.getDirective());
		}

		std::string detailedText = "";
		for (const auto& p: orderedIncludes)
		{
			detailedText += p.first + "\n";

			for (const auto& p2: p.second)
			{
				detailedText += std::to_string(p2.first) + ":\t" + p2.second + "\n";
			}

			detailedText += "\n";
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
	: QtProjectWizzardContentPaths(std::shared_ptr<SourceGroupSettings>(), window)
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
	m_list->setList(ApplicationSettings::getInstance()->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::save()
{
	ApplicationSettings::getInstance()->setHeaderSearchPaths(m_list->getList());
	ApplicationSettings::getInstance()->save();
}


QtProjectWizzardContentPathsFrameworkSearch::QtProjectWizzardContentPathsFrameworkSearch(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window, bool isCDB
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setTitleString(isCDB ? "Additional Framework Search Paths" : "Framework Search Paths");
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
		m_list->setList(cxxSettings->getFrameworkSearchPaths());
	}
}

void QtProjectWizzardContentPathsFrameworkSearch::save()
{
	std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setFrameworkSearchPaths(m_list->getList());
	}
}

bool QtProjectWizzardContentPathsFrameworkSearch::isScrollAble() const
{
	return true;
}

QtProjectWizzardContentPathsFrameworkSearchGlobal::QtProjectWizzardContentPathsFrameworkSearchGlobal(
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(std::shared_ptr<SourceGroupSettings>(), window)
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
	m_list->setList(ApplicationSettings::getInstance()->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::save()
{
	ApplicationSettings::getInstance()->setFrameworkSearchPaths(m_list->getList());
	ApplicationSettings::getInstance()->save();
}


QtProjectWizzardContentPathsClassJava::QtProjectWizzardContentPathsClassJava(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
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
	std::shared_ptr<SourceGroupSettingsJava> javaSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsJava>(m_settings);
	if (javaSettings)
	{
		m_list->setList(javaSettings->getClasspath());
		m_useJreSystemLibraryCheckBox->setChecked(javaSettings->getUseJreSystemLibrary());
	}
}

void QtProjectWizzardContentPathsClassJava::save()
{
	std::shared_ptr<SourceGroupSettingsJava> javaSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsJava>(m_settings);
	if (javaSettings)
	{
		javaSettings->setClasspath(m_list->getList());
		javaSettings->setUseJreSystemLibrary(m_useJreSystemLibraryCheckBox->isChecked());
	}
}
