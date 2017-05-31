#include "qt/window/project_wizzard/QtProjectWizzard.h"

#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QScrollArea>
#include <QSysInfo>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCDBSource.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentLanguageAndStandard.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPath.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentProjectData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSourceGroupData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentVS.h"
#include "settings/SourceGroupSettingsCxx.h"
#include "settings/SourceGroupSettingsJava.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/utility.h"
#include "utility/utilityPathDetection.h"
#include "utility/utilityString.h"
#include "utility/utilityUuid.h"

#include "Application.h"

QtProjectWizzard::QtProjectWizzard(QWidget* parent)
	: QtProjectWizzardWindow(parent, false)
	, m_windowStack(this)
	, m_editing(false)
	, m_contentWidget(nullptr)
{
	setScrollAble(true);

	connect(&m_windowStack, SIGNAL(push()), this, SLOT(windowStackChanged()));
	connect(&m_windowStack, SIGNAL(pop()), this, SLOT(windowStackChanged()));
	connect(&m_windowStack, SIGNAL(empty()), this, SLOT(windowStackChanged()));

	// save old application settings so they can be compared later
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	m_appSettings.setHeaderSearchPaths(appSettings->getHeaderSearchPaths());
	m_appSettings.setFrameworkSearchPaths(appSettings->getFrameworkSearchPaths());
}

QSize QtProjectWizzard::sizeHint() const
{
	return QSize(1000, 700);
}

void QtProjectWizzard::newProject()
{
	m_projectSettings = std::make_shared<ProjectSettings>();
	setup();
}

void QtProjectWizzard::newProjectFromCDB(const FilePath& filePath, const std::vector<FilePath>& headerPaths)
{
	if (!m_projectSettings)
	{
		m_projectSettings = std::make_shared<ProjectSettings>();
	}

	if (!m_projectSettings->getProjectName().size())
	{
		m_projectSettings->setProjectName(filePath.withoutExtension().fileName());
	}

	if (m_projectSettings->getProjectFileLocation().empty())
	{
		m_projectSettings->setProjectFileLocation(filePath.parentDirectory());
	}

	if (!m_contentWidget)
	{
		setup();
	}
	else
	{
		loadContent();
	}

	cancelSourceGroup();

	std::shared_ptr<SourceGroupSettingsCxx> sourceGroupSettings =
		std::make_shared<SourceGroupSettingsCxx>(utility::getUuidString(), SOURCE_GROUP_CXX_CDB, m_projectSettings.get());
	sourceGroupSettings->setCompilationDatabasePath(filePath);
	sourceGroupSettings->setSourcePaths(headerPaths);
	m_newSourceGroupSettings = sourceGroupSettings;

	emptySourceGroupCDBVS();
}

void QtProjectWizzard::refreshProjectFromSolution(const std::string& ideId, const std::string& solutionPath)
{
}

void QtProjectWizzard::editProject(const FilePath& settingsPath)
{
	std::shared_ptr<ProjectSettings> settings = std::make_shared<ProjectSettings>(settingsPath);
	settings->reload();
	editProject(settings);
}

void QtProjectWizzard::editProject(std::shared_ptr<ProjectSettings> settings)
{
	m_projectSettings = settings;
	m_allSourceGroupSettings = settings->getAllSourceGroupSettings();
	m_editing = true;

	setup();
}

void QtProjectWizzard::populateWindow(QWidget* widget)
{
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	{
		QWidget* menu = new QWidget();
		menu->setObjectName("project_menu");
		menu->setFixedWidth(200);
		layout->addWidget(menu);

		QVBoxLayout* menuLayout = new QVBoxLayout(menu);
		menuLayout->setContentsMargins(0, 0, 0, 0);
		menuLayout->setSpacing(0);

		m_generalButton = new QPushButton("General");
		m_generalButton->setObjectName("general_button");
		m_generalButton->setCheckable(true);
		connect(m_generalButton, SIGNAL(clicked()), this, SLOT(generalButtonClicked()));
		menuLayout->addWidget(m_generalButton);

		QLabel* sourceGroupLabel = new QLabel("Source Groups:");
		menuLayout->addWidget(sourceGroupLabel);

		m_sourceGroupList = new QListWidget();
		m_sourceGroupList->setAttribute(Qt::WA_MacShowFocusRect, 0);
		connect(m_sourceGroupList, SIGNAL(currentRowChanged(int)), this, SLOT(selectedSourceGroupChanged(int)));
		menuLayout->addWidget(m_sourceGroupList);

		QHBoxLayout* buttonsLayout = new QHBoxLayout();
		buttonsLayout->setContentsMargins(0, 0, 0, 0);
		buttonsLayout->setSpacing(0);

		QPushButton* addButton = new QtIconButton(
			(ResourcePaths::getGuiPath().str() + "window/source_group_add.png").c_str(),
			(ResourcePaths::getGuiPath().str() + "window/source_group_add_hover.png").c_str());

		m_removeButton = new QtIconButton(
			(ResourcePaths::getGuiPath().str() + "window/source_group_delete.png").c_str(),
			(ResourcePaths::getGuiPath().str() + "window/source_group_delete_hover.png").c_str());

		m_duplicateButton = new QtIconButton(
			(ResourcePaths::getGuiPath().str() + "window/source_group_copy.png").c_str(),
			(ResourcePaths::getGuiPath().str() + "window/source_group_copy_hover.png").c_str());

		addButton->setIconSize(QSize(20, 20));
		m_removeButton->setIconSize(QSize(20, 20));
		m_duplicateButton->setIconSize(QSize(20, 20));

		addButton->setToolTip("Add Source Group");
		m_removeButton->setToolTip("Remove Source Group");
		m_duplicateButton->setToolTip("Copy Source Group");

		addButton->setObjectName("action_button");
		m_removeButton->setObjectName("action_button");
		m_duplicateButton->setObjectName("action_button");

		m_removeButton->setEnabled(false);
		m_duplicateButton->setEnabled(false);

		connect(addButton, SIGNAL(clicked()), this, SLOT(newSourceGroup()));
		connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeSelectedSourceGroup()));
		connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(duplicateSelectedSourceGroup()));

		buttonsLayout->addWidget(addButton);
		buttonsLayout->addWidget(m_removeButton);
		buttonsLayout->addWidget(m_duplicateButton);
		buttonsLayout->addStretch();

		menuLayout->addLayout(buttonsLayout);
	}

	QFrame* separator = new QFrame();
	separator->setFrameShape(QFrame::VLine);

	QPalette palette = separator->palette();
	palette.setColor(QPalette::WindowText, Qt::lightGray);
	separator->setPalette(palette);

	layout->addWidget(separator);

	m_contentWidget = new QWidget();
	m_contentWidget->setObjectName("form");

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setObjectName("formArea");
	scrollArea->setWidgetResizable(true);

	scrollArea->setWidget(m_contentWidget);
	layout->addWidget(scrollArea);

	widget->setLayout(layout);
}

void QtProjectWizzard::windowReady()
{
	if (m_editing)
	{
		updateTitle("Edit Project");
		updateNextButton("Save");
	}
	else
	{
		updateTitle("New Project");
		updateNextButton("Create");
		setNextEnabled(false);
	}

	updateSubTitle("Overview");
	updatePreviousButton("Add Source Group");

	connect(this, SIGNAL(previous()), this, SLOT(newSourceGroup()));
	connect(this, SIGNAL(next()), this, SLOT(createProject()));

	updateSourceGroupList();

	if (m_allSourceGroupSettings.size())
	{
		m_sourceGroupList->setCurrentRow(0);
	}
	else
	{
		generalButtonClicked();
	}
}

void QtProjectWizzard::handlePrevious()
{
	QtWindow::handlePrevious();
}

bool QtProjectWizzard::applicationSettingsContainVisualStudioHeaderSearchPaths()
{
	std::vector<FilePath> expandedPaths;
	const std::shared_ptr<CombinedPathDetector> headerPathDetector = utility::getCxxVsHeaderPathDetector();
	for (const std::string& detectorName: headerPathDetector->getWorkingDetectorNames())
	{
		for (const FilePath& path: headerPathDetector->getPaths(detectorName))
		{
			utility::append(expandedPaths, path.expandEnvironmentVariables());
		}
	}

	std::vector<FilePath> usedExpandedGlobalHeaderSearchPaths =
		ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded();
	for (const FilePath& usedExpandedPath: usedExpandedGlobalHeaderSearchPaths)
	{
		for (const FilePath& expandedPath: expandedPaths)
		{
			if (expandedPath == usedExpandedPath)
			{
				return true;
			}
		}
	}

	return false;
}

QtProjectWizzardWindow* QtProjectWizzard::createWindowWithContent(
	std::function<QtProjectWizzardContent*(QtProjectWizzardWindow*)> func
){
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelSourceGroup()));

	window->setPreferredSize(QSize(580, 340));
	window->setContent(func(window));
	window->setScrollAble(window->content()->isScrollAble());
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

QtProjectWizzardWindow* QtProjectWizzard::createWindowWithSummary(
	std::function<void(QtProjectWizzardWindow*, QtProjectWizzardContentSummary*)> func
){
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, SIGNAL(previous()), &m_windowStack, SLOT(popWindow()));
	connect(window, SIGNAL(canceled()), this, SLOT(cancelSourceGroup()));

	QtProjectWizzardContentSummary* summary = new QtProjectWizzardContentSummary(window);

	window->setPreferredSize(QSize(750, 500));
	window->setContent(summary);
	window->setScrollAble(window->content()->isScrollAble());
	func(window, summary);
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

void QtProjectWizzard::updateSourceGroupList()
{
	m_sourceGroupList->clear();

	for (const std::shared_ptr<SourceGroupSettings> group : m_allSourceGroupSettings)
	{
		QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(group->getName()));
		m_sourceGroupList->addItem(item);
	}
}

bool QtProjectWizzard::canExitContent()
{
	if (content())
	{
		if (!content()->check())
		{
			return false;
		}

		saveContent();
	}

	return true;
}

void QtProjectWizzard::generalButtonClicked()
{
	if (!canExitContent())
	{
		m_generalButton->setChecked(false);
		return;
	}

	setContent(new QtProjectWizzardContentProjectData(m_projectSettings, this, m_editing));

	qDeleteAll(m_contentWidget->children());
	QtProjectWizzardWindow::populateWindow(m_contentWidget);

	loadContent();

	m_generalButton->setChecked(true);
	m_sourceGroupList->setCurrentRow(-1);
}

void QtProjectWizzard::selectedSourceGroupChanged(int index)
{
	if (index < 0 || index >= int(m_allSourceGroupSettings.size()))
	{
		m_removeButton->setEnabled(false);
		m_duplicateButton->setEnabled(false);
		return;
	}

	if (!canExitContent())
	{
		return;
	}

	m_generalButton->setChecked(false);
	m_removeButton->setEnabled(true);
	m_duplicateButton->setEnabled(true);

	std::shared_ptr<SourceGroupSettings> group = m_allSourceGroupSettings[index];

	QtProjectWizzardContentSummary* summary = new QtProjectWizzardContentSummary(this);
	summary->setIsForm(true);

	QtProjectWizzardContentSourceGroupData* content = new QtProjectWizzardContentSourceGroupData(group, this);
	connect(content, SIGNAL(nameUpdated(QString)), this, SLOT(selectedSourceGroupNameChanged(QString)));
	summary->addContent(content);
	summary->addSpace();

	if (group->getType() == SOURCE_GROUP_JAVA_EMPTY || group->getType() == SOURCE_GROUP_JAVA_MAVEN)
	{
		bool isMaven = false;
		std::shared_ptr<SourceGroupSettingsJava> javaSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsJava>(group);
		if (javaSettings)
		{
			isMaven = javaSettings->getMavenProjectFilePathExpandedAndAbsolute().exists();
		}

		summary->addContent(new QtProjectWizzardContentLanguageAndStandard(group, this));
		summary->addSpace();

		if (isMaven)
		{
			summary->addContent(new QtProjectWizzardContentPathSourceMaven(group, this));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathDependenciesMaven(group, this));
		}
		else
		{
			summary->addContent(new QtProjectWizzardContentPathsSource(group, this));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsClassJava(group, this));
		}

		summary->addSpace();
		summary->addContent(new QtProjectWizzardContentExtensions(group, this));
		summary->addSpace();
		summary->addContent(new QtProjectWizzardContentPathsExclude(group, this));
	}
	else
	{
		const bool isCDB = group->getType() == SOURCE_GROUP_CXX_CDB || group->getType() == SOURCE_GROUP_CXX_CDB;

		if (isCDB)
		{
			summary->addContent(new QtProjectWizzardContentPathCDB(group, this));
			summary->addContent(new QtProjectWizzardContentCDBSource(group, this));
			summary->addSpace();

			summary->addContent(new QtProjectWizzardContentPathsCDBHeader(group, this));
			summary->addSpace();
		}
		else
		{
			summary->addContent(new QtProjectWizzardContentLanguageAndStandard(group, this));
			summary->addSpace();

			summary->addContent(new QtProjectWizzardContentPathsSource(group, this));
			summary->addContent(new QtProjectWizzardContentExtensions(group, this));
			summary->addSpace();
		}

		summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(group, this, isCDB));

		std::shared_ptr<SourceGroupSettingsCxx> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(group);
		if (!isCDB && cxxSettings && cxxSettings->getHasDefinedUseSourcePathsForHeaderSearch())
		{
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentSimple(group, this));
		}

		summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(this));
		summary->addSpace();

		if (QSysInfo::macVersion() != QSysInfo::MV_None)
		{
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(group, this, isCDB));
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(this));
			summary->addSpace();
		}

		summary->addContent(new QtProjectWizzardContentFlags(group, this));
		summary->addSpace();
		summary->addContent(new QtProjectWizzardContentPathsExclude(group, this));
	}

	setContent(summary);

	qDeleteAll(m_contentWidget->children());
	QtProjectWizzardWindow::populateWindow(m_contentWidget);

	loadContent();
}

void QtProjectWizzard::selectedSourceGroupNameChanged(QString name)
{
	m_sourceGroupList->item(m_sourceGroupList->currentRow())->setText(name);
}

void QtProjectWizzard::removeSelectedSourceGroup()
{
	if (!m_allSourceGroupSettings.size() || m_sourceGroupList->currentRow() < 0)
	{
		return;
	}

	QMessageBox msgBox;
	msgBox.setText("Remove Source Group");
	msgBox.setInformativeText("Do you really want to remove this source group from the project?");
	msgBox.addButton("Yes", QMessageBox::ButtonRole::YesRole);
	msgBox.addButton("No", QMessageBox::ButtonRole::NoRole);
	msgBox.setIcon(QMessageBox::Icon::Question);
	int ret = msgBox.exec();

	if (ret == 0) // QMessageBox::Yes
	{
		int currentRow = m_sourceGroupList->currentRow();
		m_allSourceGroupSettings.erase(m_allSourceGroupSettings.begin() + currentRow);
		updateSourceGroupList();

		if (!m_allSourceGroupSettings.size())
		{
			setNextEnabled(false);
			generalButtonClicked();
			return;
		}

		if (currentRow >= int(m_allSourceGroupSettings.size()))
		{
			currentRow--;
		}

		m_sourceGroupList->setCurrentRow(currentRow);
	}
}

void QtProjectWizzard::duplicateSelectedSourceGroup()
{
	if (!m_allSourceGroupSettings.size() || m_sourceGroupList->currentRow() < 0 || !canExitContent())
	{
		return;
	}

	std::shared_ptr<SourceGroupSettings> oldSourceGroup = m_allSourceGroupSettings[m_sourceGroupList->currentRow()];
	std::shared_ptr<SourceGroupSettings> newSourceGroup;

	if (oldSourceGroup->getType() == SOURCE_GROUP_JAVA_EMPTY || oldSourceGroup->getType() == SOURCE_GROUP_JAVA_MAVEN)
	{
		newSourceGroup = std::make_shared<SourceGroupSettingsJava>(*dynamic_cast<SourceGroupSettingsJava*>(oldSourceGroup.get()));
	}
	else
	{
		newSourceGroup = std::make_shared<SourceGroupSettingsCxx>(*dynamic_cast<SourceGroupSettingsCxx*>(oldSourceGroup.get()));
	}

	newSourceGroup->setId(utility::getUuidString());

	int newRow = m_sourceGroupList->currentRow() + 1;
	m_allSourceGroupSettings.insert(m_allSourceGroupSettings.begin() + newRow, newSourceGroup);

	updateSourceGroupList();
	m_sourceGroupList->setCurrentRow(newRow);
}

void QtProjectWizzard::cancelSourceGroup()
{
	m_windowStack.clearWindows();
	m_newSourceGroupSettings.reset();
}

void QtProjectWizzard::cancelWizzard()
{
	m_windowStack.clearWindows();
	emit canceled();
}

void QtProjectWizzard::finishWizzard()
{
	m_windowStack.clearWindows();
	emit finished();
}

void QtProjectWizzard::windowStackChanged()
{
	QWidget* window = m_windowStack.getTopWindow();

	if (window)
	{
		dynamic_cast<QtProjectWizzardWindow*>(window)->content()->load();
		setEnabled(false);
	}
	else
	{
		setEnabled(true);
		raise();
	}
}

void QtProjectWizzard::newSourceGroup()
{
	if (!canExitContent())
	{
		return;
	}

	QtProjectWizzardWindow* window = createWindowWithContent(
		[this](QtProjectWizzardWindow* window)
		{
			window->setPreferredSize(QSize(570, 420));
			return new QtProjectWizzardContentSelect(window);
		}
	);

	connect(dynamic_cast<QtProjectWizzardContentSelect*>(window->content()),
		SIGNAL(selected(SourceGroupType)),
		this, SLOT(selectedProjectType(SourceGroupType)));

	window->show();
	window->setNextEnabled(false);
	window->setPreviousEnabled(false);
	window->updateSubTitle("Type Selection");
}

void QtProjectWizzard::selectedProjectType(SourceGroupType sourceGroupType)
{
	const std::string sourceGroupId = utility::getUuidString();

	switch (sourceGroupType)
	{
	case SOURCE_GROUP_C_EMPTY:
	case SOURCE_GROUP_CPP_EMPTY:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsCxx>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		if (applicationSettingsContainVisualStudioHeaderSearchPaths())
		{
			std::vector<std::string> flags;
			flags.push_back("-fms-extensions");
			flags.push_back("-fms-compatibility");
			flags.push_back("-fms-compatibility-version=19");
			std::dynamic_pointer_cast<SourceGroupSettingsCxx>(m_newSourceGroupSettings)->setCompilerFlags(flags);
		}
		emptySourceGroup();
		break;
	case SOURCE_GROUP_CXX_CDB:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsCxx>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		emptySourceGroupCDB();
		break;
	case SOURCE_GROUP_CXX_VS:
		m_newSourceGroupSettings =
			std::make_shared<SourceGroupSettingsCxx>(sourceGroupId, SOURCE_GROUP_CXX_CDB, m_projectSettings.get());
		emptySourceGroupCDBVS();
		break;
	case SOURCE_GROUP_JAVA_EMPTY:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsJava>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		emptySourceGroup();
		break;
	case SOURCE_GROUP_JAVA_MAVEN:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsJava>(sourceGroupId, sourceGroupType, m_projectSettings.get());
		sourcePathsJavaMaven();
		break;
	case SOURCE_GROUP_UNKNOWN:
		break;
	}
}

void QtProjectWizzard::emptySourceGroup()
{
	QtProjectWizzardWindow* window = createWindowWithContent(
		[this](QtProjectWizzardWindow* window)
		{
			return new QtProjectWizzardContentLanguageAndStandard(m_newSourceGroupSettings, window);
		}
	);

	if (m_newSourceGroupSettings->getType() == SOURCE_GROUP_JAVA_EMPTY)
	{
		connect(window, SIGNAL(next()), this, SLOT(sourcePathsJava()));
	}
	else
	{
		connect(window, SIGNAL(next()), this, SLOT(sourcePaths()));
	}

	window->updateSubTitle("Standard");
}

void QtProjectWizzard::emptySourceGroupCDBVS()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentVS(window));
			summary->addContent(new QtProjectWizzardContentPathCDB(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentCDBSource(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsCDBHeader(m_newSourceGroupSettings, window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsCxx()));
	window->updateSubTitle("VS Solution");
}

void QtProjectWizzard::emptySourceGroupCDB()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathCDB(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentCDBSource(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsCDBHeader(m_newSourceGroupSettings, window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsCxx()));
	window->updateSubTitle("CDB Path");
}

void QtProjectWizzard::sourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsSource(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentExtensions(m_newSourceGroupSettings, window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerSearchPaths()));
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::headerSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(headerSearchPathsDone()));
	window->updateSubTitle("Include Paths");
}

void QtProjectWizzard::headerSearchPathsDone()
{
	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		frameworkSearchPaths();
	}
	else
	{
		advancedSettingsCxx();
	}
}

void QtProjectWizzard::frameworkSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsCxx()));
	window->updateSubTitle("Framework Search Paths");
}

void QtProjectWizzard::sourcePathsJava()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathsSource(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsClassJava(m_newSourceGroupSettings, window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsJava()));
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::sourcePathsJavaMaven()
{
	std::dynamic_pointer_cast<SourceGroupSettingsJava>(m_newSourceGroupSettings)->setMavenDependenciesDirectory(
		FilePath("./sourcetrail_dependencies/" + utility::replace(m_projectSettings->getProjectName(), " ", "_") + "/maven")
	);

	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentPathSourceMaven(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathDependenciesMaven(m_newSourceGroupSettings, window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(advancedSettingsJava()));
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::advancedSettingsCxx()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentFlags(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(m_newSourceGroupSettings, window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(createSourceGroup()));
	window->updateSubTitle("Advanced (optional)");
}

void QtProjectWizzard::advancedSettingsJava()
{
	QtProjectWizzardWindow* window = createWindowWithSummary(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentSummary* summary)
		{
			summary->addContent(new QtProjectWizzardContentExtensions(m_newSourceGroupSettings, window));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(m_newSourceGroupSettings, window));
		}
	);

	connect(window, SIGNAL(next()), this, SLOT(createSourceGroup()));
	window->updateSubTitle("Advanced (optional)");
}

void QtProjectWizzard::createSourceGroup()
{
	m_allSourceGroupSettings.push_back(m_newSourceGroupSettings);
	m_newSourceGroupSettings.reset();

	updateSourceGroupList();
	cancelSourceGroup();

	setNextEnabled(true);

	m_sourceGroupList->setCurrentRow(m_allSourceGroupSettings.size() - 1);
}

void QtProjectWizzard::createProject()
{
	FilePath path = m_projectSettings->getFilePath();

	m_projectSettings->setVersion(ProjectSettings::VERSION);
	m_projectSettings->setAllSourceGroupSettings(m_allSourceGroupSettings);
	m_projectSettings->save(path);

	bool forceRefreshProject = false;
	if (m_editing)
	{
		bool settingsChanged = false;

		Application* application = Application::getInstance().get();
		if (application->getCurrentProject() != NULL)
		{
			settingsChanged = !(application->getCurrentProject()->settingsEqualExceptNameAndLocation(*(m_projectSettings.get())));
		}

		bool appSettingsChanged = !(m_appSettings == *ApplicationSettings::getInstance().get());

		if (settingsChanged || appSettingsChanged)
		{
			forceRefreshProject = true;
		}
	}
	else
	{
		MessageStatus("Created project: " + path.str()).dispatch();
	}

	MessageLoadProject(path, forceRefreshProject).dispatch();

	finishWizzard();
}
