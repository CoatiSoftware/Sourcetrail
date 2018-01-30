#include "qt/window/project_wizzard/QtProjectWizzard.h"

#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QScrollArea>
#include <QSysInfo>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCDBSource.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCrossCompilationOptions.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentLanguageAndStandard.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPath.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentProjectData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSourceGroupData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSourceGroupInfoText.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentGroup.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentVS.h"
#include "settings/SourceGroupSettingsCxxEmpty.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsJavaEmpty.h"
#include "settings/SourceGroupSettingsJavaGradle.h"
#include "settings/SourceGroupSettingsJavaMaven.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/utility.h"
#include "utility/utilityApp.h"
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

	connect(&m_windowStack, &QtWindowStack::push, this, &QtProjectWizzard::windowStackChanged);
	connect(&m_windowStack, &QtWindowStack::pop, this, &QtProjectWizzard::windowStackChanged);
	connect(&m_windowStack, &QtWindowStack::empty, this, &QtProjectWizzard::windowStackChanged);

	// save old application settings so they can be compared later
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	m_appSettings.setHeaderSearchPaths(appSettings->getHeaderSearchPaths());
	m_appSettings.setFrameworkSearchPaths(appSettings->getFrameworkSearchPaths());
}

QSize QtProjectWizzard::sizeHint() const
{
	return QSize(900, 600);
}

void QtProjectWizzard::newProject()
{
	m_projectSettings = std::make_shared<ProjectSettings>();
	setup();
}

void QtProjectWizzard::newProjectFromCDB(const FilePath& filePath)
{
	if (!m_projectSettings)
	{
		m_projectSettings = std::make_shared<ProjectSettings>();
	}

	if (m_projectSettings->getProjectFilePath().empty())
	{
		m_projectSettings->setProjectFilePath(filePath.withoutExtension().wFileName(), filePath.getParentDirectory());
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

	std::shared_ptr<SourceGroupSettingsCxxCdb> sourceGroupSettings =
		std::make_shared<SourceGroupSettingsCxxCdb>(utility::getUuidString(), m_projectSettings.get());
	sourceGroupSettings->setCompilationDatabasePath(filePath);
	m_newSourceGroupSettings = sourceGroupSettings;

	emptySourceGroupCDBVS();
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
		connect(m_generalButton, &QPushButton::clicked, this, &QtProjectWizzard::generalButtonClicked);
		menuLayout->addWidget(m_generalButton);

		QLabel* sourceGroupLabel = new QLabel("Source Groups:");
		menuLayout->addWidget(sourceGroupLabel);

		m_sourceGroupList = new QListWidget();
		m_sourceGroupList->setAttribute(Qt::WA_MacShowFocusRect, 0);
		connect(m_sourceGroupList, &QListWidget::currentRowChanged, this, &QtProjectWizzard::selectedSourceGroupChanged);
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

		connect(addButton, &QPushButton::clicked, this, &QtProjectWizzard::newSourceGroup);
		connect(m_removeButton, &QPushButton::clicked, this, &QtProjectWizzard::removeSelectedSourceGroup);
		connect(m_duplicateButton, &QPushButton::clicked, this, &QtProjectWizzard::duplicateSelectedSourceGroup);

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
	layout->addSpacing(10);

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

	connect(this, &QtProjectWizzard::previous, this, &QtProjectWizzard::newSourceGroup);
	connect(this, &QtProjectWizzard::next, this, &QtProjectWizzard::createProject);

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
	for (const std::string& detectorName : headerPathDetector->getWorkingDetectorNames())
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

	connect(window, &QtProjectWizzardWindow::previous, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtProjectWizzardWindow::canceled, this, &QtProjectWizzard::cancelSourceGroup);

	window->setPreferredSize(QSize(580, 340));
	window->setContent(func(window));
	window->setScrollAble(window->content()->isScrollAble());
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

QtProjectWizzardWindow* QtProjectWizzard::createWindowWithContentGroup(
	std::function<void(QtProjectWizzardWindow*, QtProjectWizzardContentGroup*)> func
){
	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(parentWidget());

	connect(window, &QtProjectWizzardWindow::previous, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtProjectWizzardWindow::canceled, this, &QtProjectWizzard::cancelSourceGroup);

	QtProjectWizzardContentGroup* contentGroup = new QtProjectWizzardContentGroup(window);

	window->setPreferredSize(QSize(750, 600));
	window->setContent(contentGroup);
	window->setScrollAble(window->content()->isScrollAble());
	func(window, contentGroup);
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

void QtProjectWizzard::updateSourceGroupList()
{
	m_sourceGroupList->clear();

	for (const std::shared_ptr<SourceGroupSettings>& group : m_allSourceGroupSettings)
	{
		QString name = QString::fromStdString(group->getName());
		if (group->getStatus() == SOURCE_GROUP_STATUS_DISABLED)
		{
			name = "(" + name + ")";
		}

		QListWidgetItem *item = new QListWidgetItem(name);
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

	QtProjectWizzardContentGroup* contentGroup = new QtProjectWizzardContentGroup(this);
	contentGroup->addContent(new QtProjectWizzardContentProjectData(m_projectSettings, this, m_editing));

	if (m_allSourceGroupSettings.empty())
	{
		contentGroup->addSpace();
		contentGroup->addContent(new QtProjectWizzardContentSourceGroupInfoText(this));
	}

	setContent(contentGroup);

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

	QtProjectWizzardContentGroup* summary = new QtProjectWizzardContentGroup(this);
	summary->setIsForm(true);

	QtProjectWizzardContentSourceGroupData* content = new QtProjectWizzardContentSourceGroupData(group, this);
	connect(content, &QtProjectWizzardContentSourceGroupData::nameUpdated, this, &QtProjectWizzard::selectedSourceGroupNameChanged);
	summary->addContent(content);
	summary->addSpace();

	if (std::shared_ptr<SourceGroupSettingsJava> settingsJava = std::dynamic_pointer_cast<SourceGroupSettingsJava>(group))
	{
		summary->addContent(new QtProjectWizzardContentLanguageAndStandard(group, this));
		summary->addSpace();

		if (std::shared_ptr<SourceGroupSettingsJavaEmpty> settingsJavaEmpty = std::dynamic_pointer_cast<SourceGroupSettingsJavaEmpty>(group))
		{
			summary->addContent(new QtProjectWizzardContentPathsSource(group, this));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(group, this));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentExtensions(group, this));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsClassJava(group, this));
		}
		else
		{
			if (std::shared_ptr<SourceGroupSettingsJavaGradle> settingsJavaGradle = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(group))
			{
				summary->addContent(new QtProjectWizzardContentPathSourceGradle(group, this));
				summary->addSpace();
				summary->addContent(new QtProjectWizzardContentPathDependenciesGradle(group, this));
			}
			else if (std::shared_ptr<SourceGroupSettingsJavaMaven> settingsJavaMaven = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(group))
			{
				summary->addContent(new QtProjectWizzardContentPathSourceMaven(group, this));
				summary->addSpace();
				summary->addContent(new QtProjectWizzardContentPathDependenciesMaven(group, this));
			}

			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentPathsExclude(group, this));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentExtensions(group, this));
		}
	}
	else if (std::shared_ptr<SourceGroupSettingsCxx> settingsCxx = std::dynamic_pointer_cast<SourceGroupSettingsCxx>(group))
	{
		bool isCDB = false;
		if (std::shared_ptr<SourceGroupSettingsCxxEmpty> settingsCxxEmpty = std::dynamic_pointer_cast<SourceGroupSettingsCxxEmpty>(group))
		{
			summary->addContent(new QtProjectWizzardContentLanguageAndStandard(group, this));
			summary->addSpace();
			summary->addContent(new QtProjectWizzardContentCrossCompilationOptions(group, this));
			summary->addSpace();

			summary->addContent(new QtProjectWizzardContentPathsSource(group, this));
			summary->addContent(new QtProjectWizzardContentPathsExclude(group, this));
			summary->addContent(new QtProjectWizzardContentExtensions(group, this));
			summary->addSpace();
		}
		else if (std::shared_ptr<SourceGroupSettingsCxxCdb> settingsCxxCdb = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(group))
		{
			isCDB = true;

			summary->addContent(new QtProjectWizzardContentPathCDB(group, this));
			summary->addContent(new QtProjectWizzardContentCDBSource(group, this));
			summary->addSpace();

			summary->addContent(new QtProjectWizzardContentPathsCDBHeader(group, this));
			summary->addContent(new QtProjectWizzardContentPathsExclude(group, this));
			summary->addSpace();
		}

		summary->addContent(new QtProjectWizzardContentPathsHeaderSearch(group, this, isCDB));

		summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(this));
		summary->addSpace();

		if (utility::getOsType() == OS_MAC)
		{
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearch(group, this, isCDB));
			summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(this));
			summary->addSpace();
		}

		summary->addContent(new QtProjectWizzardContentFlags(group, this, isCDB));
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
	switch (oldSourceGroup->getType())
	{
	case SOURCE_GROUP_JAVA_EMPTY:
		newSourceGroup = std::make_shared<SourceGroupSettingsJavaEmpty>(*dynamic_cast<SourceGroupSettingsJavaEmpty*>(oldSourceGroup.get()));
		break;
	case SOURCE_GROUP_JAVA_MAVEN:
		newSourceGroup = std::make_shared<SourceGroupSettingsJavaMaven>(*dynamic_cast<SourceGroupSettingsJavaMaven*>(oldSourceGroup.get()));
		break;
	case SOURCE_GROUP_JAVA_GRADLE:
		newSourceGroup = std::make_shared<SourceGroupSettingsJavaGradle>(*dynamic_cast<SourceGroupSettingsJavaGradle*>(oldSourceGroup.get()));
		break;
	case SOURCE_GROUP_C_EMPTY:
	case SOURCE_GROUP_CPP_EMPTY:
		newSourceGroup = std::make_shared<SourceGroupSettingsCxxEmpty>(*dynamic_cast<SourceGroupSettingsCxxEmpty*>(oldSourceGroup.get()));
		break;
	case SOURCE_GROUP_CXX_CDB:
		newSourceGroup = std::make_shared<SourceGroupSettingsCxxCdb>(*dynamic_cast<SourceGroupSettingsCxxCdb*>(oldSourceGroup.get()));
		break;
	default:
		return;
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
			window->setPreferredSize(QSize(570, 380));
			return new QtProjectWizzardContentSelect(window);
		}
	);

	connect(dynamic_cast<QtProjectWizzardContentSelect*>(window->content()),
		&QtProjectWizzardContentSelect::selected,
		this, &QtProjectWizzard::selectedProjectType);

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
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsCxxEmpty>(sourceGroupId, sourceGroupType, m_projectSettings.get());
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
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsCxxCdb>(sourceGroupId, m_projectSettings.get());
		emptySourceGroupCDB();
		break;
	case SOURCE_GROUP_CXX_VS:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsCxxCdb>(sourceGroupId, m_projectSettings.get());
		emptySourceGroupCDBVS();
		break;
	case SOURCE_GROUP_JAVA_EMPTY:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsJavaEmpty>(sourceGroupId, m_projectSettings.get());
		emptySourceGroup();
		break;
	case SOURCE_GROUP_JAVA_MAVEN:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsJavaMaven>(sourceGroupId, m_projectSettings.get());
		sourcePathsJavaMaven();
		break;
	case SOURCE_GROUP_JAVA_GRADLE:
		m_newSourceGroupSettings = std::make_shared<SourceGroupSettingsJavaGradle>(sourceGroupId, m_projectSettings.get());
		sourcePathsJavaGradle();
		break;
	case SOURCE_GROUP_UNKNOWN:
		break;
	}
}

void QtProjectWizzard::emptySourceGroup()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
		contentGroup->addContent(new QtProjectWizzardContentLanguageAndStandard(m_newSourceGroupSettings, window));
			if (m_newSourceGroupSettings->getType() == SOURCE_GROUP_C_EMPTY||
				m_newSourceGroupSettings->getType() == SOURCE_GROUP_CPP_EMPTY)
			{
				contentGroup->addContent(new QtProjectWizzardContentCrossCompilationOptions(m_newSourceGroupSettings, window));
				window->setPreferredSize(QSize(470, 460));
			}
			else
			{
				window->setPreferredSize(QSize(470, 230));
			}
		}
	);

	if (m_newSourceGroupSettings->getType() == SOURCE_GROUP_JAVA_EMPTY)
	{
		connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::sourcePathsJava);
	}
	else
	{
		connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::sourcePaths);
	}

	window->updateSubTitle("Standard");
}

void QtProjectWizzard::emptySourceGroupCDBVS()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentVS(window));
			contentGroup->addContent(new QtProjectWizzardContentPathCDB(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentCDBSource(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsCDBHeader(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsExclude(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::createSourceGroup);
	window->updateSubTitle("VS Solution");
}

void QtProjectWizzard::emptySourceGroupCDB()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathCDB(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentCDBSource(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsCDBHeader(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsExclude(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::createSourceGroup);
	window->updateSubTitle("CDB Path");
}

void QtProjectWizzard::sourcePaths()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathsSource(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsExclude(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentExtensions(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::headerSearchPaths);
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::headerSearchPaths()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathsHeaderSearch(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::headerSearchPathsDone);
	window->updateSubTitle("Include Paths");
}

void QtProjectWizzard::headerSearchPathsDone()
{
	if (utility::getOsType() == OS_MAC)
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
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathsFrameworkSearch(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::advancedSettingsCxx);
	window->updateSubTitle("Framework Search Paths");
}

void QtProjectWizzard::sourcePathsJava()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathsSource(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathsExclude(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentExtensions(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::dependenciesJava);
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::dependenciesJava()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathsClassJava(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::createSourceGroup);
	window->updateSubTitle("Dependencies");
}

void QtProjectWizzard::sourcePathsJavaMaven()
{
	std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(m_newSourceGroupSettings)->setMavenDependenciesDirectory(
		FilePath(
			L"./sourcetrail_dependencies/" + utility::replace(m_projectSettings->getProjectName(), L" ", L"_") + 
			L"/" + 
			utility::decodeFromUtf8(m_newSourceGroupSettings->getId()) +
			L"/maven"
		)
	);

	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathSourceMaven(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathDependenciesMaven(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::advancedSettingsJava);
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::sourcePathsJavaGradle()
{
	std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(m_newSourceGroupSettings)->setGradleDependenciesDirectory(
		FilePath(
			L"./sourcetrail_dependencies/" + utility::replace(m_projectSettings->getProjectName(), L" ", L"_") + 
			L"/" + 
			utility::decodeFromUtf8(m_newSourceGroupSettings->getId()) +
			L"/gradle"
		)
	);

	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathSourceGradle(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentPathDependenciesGradle(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::advancedSettingsJava);
	window->updateSubTitle("Indexed Paths");
}

void QtProjectWizzard::advancedSettingsCxx()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentFlags(m_newSourceGroupSettings, window));
		}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::createSourceGroup);
	window->updateSubTitle("Advanced (optional)");
}

void QtProjectWizzard::advancedSettingsJava()
{
	QtProjectWizzardWindow* window = createWindowWithContentGroup(
		[this](QtProjectWizzardWindow* window, QtProjectWizzardContentGroup* contentGroup)
		{
			contentGroup->addContent(new QtProjectWizzardContentPathsExclude(m_newSourceGroupSettings, window));
			contentGroup->addSpace();
			contentGroup->addContent(new QtProjectWizzardContentExtensions(m_newSourceGroupSettings, window));
	}
	);

	connect(window, &QtProjectWizzardWindow::next, this, &QtProjectWizzard::createSourceGroup);
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

	bool settingsChanged = false;
	if (m_editing)
	{
		Application* application = Application::getInstance().get();
		if (application->getCurrentProject() != NULL)
		{
			settingsChanged = !(application->getCurrentProject()->settingsEqualExceptNameAndLocation(*(m_projectSettings.get())));
		}

		settingsChanged |= !(m_appSettings == *ApplicationSettings::getInstance().get());
	}
	else
	{
		MessageStatus(L"Created project: " + path.wstr()).dispatch();
	}

	MessageLoadProject(path, settingsChanged).dispatch();

	finishWizzard();
}
