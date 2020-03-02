#include "QtProjectWizard.h"

#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QScrollArea>
#include <QSysInfo>
#include <QTimer>

#include "language_packages.h"

#include "MessageLoadProject.h"
#include "MessageStatus.h"
#include "QtProjectWizardContent.h"
#include "QtProjectWizardContentCustomCommand.h"
#include "QtProjectWizardContentExtensions.h"
#include "QtProjectWizardContentGroup.h"
#include "QtProjectWizardContentPathsExclude.h"
#include "QtProjectWizardContentPathsSource.h"
#include "QtProjectWizardContentProjectData.h"
#include "QtProjectWizardContentRequiredLabel.h"
#include "QtProjectWizardContentSelect.h"
#include "QtProjectWizardContentSourceGroupData.h"
#include "QtProjectWizardContentSourceGroupInfoText.h"
#include "QtProjectWizardContentUnloadable.h"
#include "ResourcePaths.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SourceGroupSettingsUnloadable.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityPathDetection.h"
#include "utilityString.h"
#include "utilityUuid.h"

#include "Application.h"

#if BUILD_CXX_LANGUAGE_PACKAGE
#	include "QtProjectWizardContentCStandard.h"
#	include "QtProjectWizardContentCppStandard.h"
#	include "QtProjectWizardContentCrossCompilationOptions.h"
#	include "QtProjectWizardContentCxxPchFlags.h"
#	include "QtProjectWizardContentFlags.h"
#	include "QtProjectWizardContentPathCDB.h"
#	include "QtProjectWizardContentPathCodeblocksProject.h"
#	include "QtProjectWizardContentPathCxxPch.h"
#	include "QtProjectWizardContentPathsFrameworkSearch.h"
#	include "QtProjectWizardContentPathsFrameworkSearchGlobal.h"
#	include "QtProjectWizardContentPathsHeaderSearch.h"
#	include "QtProjectWizardContentPathsHeaderSearchGlobal.h"
#	include "QtProjectWizardContentPathsIndexedHeaders.h"
#	include "QtProjectWizardContentVS.h"
#	include "SourceGroupSettingsCEmpty.h"
#	include "SourceGroupSettingsCppEmpty.h"
#	include "SourceGroupSettingsCxxCdb.h"
#	include "SourceGroupSettingsCxxCodeblocks.h"
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
#	include "QtProjectWizardContentJavaStandard.h"
#	include "QtProjectWizardContentPathSettingsMaven.h"
#	include "QtProjectWizardContentPathSourceGradle.h"
#	include "QtProjectWizardContentPathSourceMaven.h"
#	include "QtProjectWizardContentPathsClassJava.h"
#	include "SourceGroupSettingsJavaEmpty.h"
#	include "SourceGroupSettingsJavaGradle.h"
#	include "SourceGroupSettingsJavaMaven.h"
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

#if BUILD_PYTHON_LANGUAGE_PACKAGE
#	include "QtProjectWizardContentPathPythonEnvironment.h"
#	include "SourceGroupSettingsPythonEmpty.h"
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

namespace
{
#if BUILD_CXX_LANGUAGE_PACKAGE

bool applicationSettingsContainVisualStudioHeaderSearchPaths()
{
	std::vector<FilePath> expandedPaths;
	const std::shared_ptr<CombinedPathDetector> headerPathDetector =
		utility::getCxxVsHeaderPathDetector();
	for (const std::string& detectorName: headerPathDetector->getWorkingDetectorNames())
	{
		for (const FilePath& path: headerPathDetector->getPathsForDetector(detectorName))
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

void addMsvcCompatibilityFlagsOnDemand(std::shared_ptr<SourceGroupSettingsWithCxxPathsAndFlags> settings)
{
	if (applicationSettingsContainVisualStudioHeaderSearchPaths())
	{
		std::vector<std::wstring> flags = settings->getCompilerFlags();
		flags.push_back(L"-fms-extensions");
		flags.push_back(L"-fms-compatibility");
		flags.push_back(L"-fms-compatibility-version=19");
		settings->setCompilerFlags(flags);
	}
}

#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

template <typename SettingsType>
void addSourceGroupContents(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SettingsType> settings,
	QtProjectWizardWindow* window);

#if BUILD_CXX_LANGUAGE_PACKAGE

template <>
void addSourceGroupContents<SourceGroupSettingsCEmpty>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsCEmpty> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentCStandard(settings, window));
	group->addContent(new QtProjectWizardContentCrossCompilationOptions(settings, window));
	group->addSpace();

	group->addContent(new QtProjectWizardContentPathsSource(settings, window));
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
	group->addSpace();

	group->addContent(new QtProjectWizardContentPathsHeaderSearch(settings, window));
	group->addContent(new QtProjectWizardContentPathsHeaderSearchGlobal(window));
	group->addSpace();

	if (utility::getOsType() == OS_MAC)
	{
		group->addContent(new QtProjectWizardContentPathsFrameworkSearch(settings, window));
		group->addContent(new QtProjectWizardContentPathsFrameworkSearchGlobal(window));
		group->addSpace();
	}

	group->addContent(new QtProjectWizardContentFlags(settings, window));
	group->addContent(new QtProjectWizardContentPathCxxPch(settings, settings, window));
	group->addContent(new QtProjectWizardContentCxxPchFlags(settings, window, false));
}

template <>
void addSourceGroupContents<SourceGroupSettingsCppEmpty>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsCppEmpty> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentCppStandard(settings, window));
	group->addContent(new QtProjectWizardContentCrossCompilationOptions(settings, window));
	group->addSpace();

	group->addContent(new QtProjectWizardContentPathsSource(settings, window));
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
	group->addSpace();

	group->addContent(new QtProjectWizardContentPathsHeaderSearch(settings, window));
	group->addContent(new QtProjectWizardContentPathsHeaderSearchGlobal(window));
	group->addSpace();

	if (utility::getOsType() == OS_MAC)
	{
		group->addContent(new QtProjectWizardContentPathsFrameworkSearch(settings, window));
		group->addContent(new QtProjectWizardContentPathsFrameworkSearchGlobal(window));
		group->addSpace();
	}

	group->addContent(new QtProjectWizardContentFlags(settings, window));
	group->addContent(new QtProjectWizardContentPathCxxPch(settings, settings, window));
	group->addContent(new QtProjectWizardContentCxxPchFlags(settings, window, false));
}

template <>
void addSourceGroupContents<SourceGroupSettingsCxxCdb>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsCxxCdb> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentPathCDB(settings, window));
	group->addContent(
		new QtProjectWizardContentPathsIndexedHeaders(settings, window, "Compilation Database"));
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addSpace();

	group->addContent(new QtProjectWizardContentPathsHeaderSearch(settings, window, true));
	group->addContent(new QtProjectWizardContentPathsHeaderSearchGlobal(window));
	group->addSpace();

	if (utility::getOsType() == OS_MAC)
	{
		group->addContent(new QtProjectWizardContentPathsFrameworkSearch(settings, window, true));
		group->addContent(new QtProjectWizardContentPathsFrameworkSearchGlobal(window));
		group->addSpace();
	}

	group->addContent(new QtProjectWizardContentFlags(settings, window, true));
	group->addContent(new QtProjectWizardContentPathCxxPch(settings, settings, window));
	group->addContent(new QtProjectWizardContentCxxPchFlags(settings, window, true));
}

template <>
void addSourceGroupContents<SourceGroupSettingsCxxCodeblocks>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentCppStandard(settings, window));
	group->addContent(new QtProjectWizardContentCStandard(settings, window));
	group->addContent(new QtProjectWizardContentPathCodeblocksProject(settings, window));
	group->addSpace();

	group->addContent(
		new QtProjectWizardContentPathsIndexedHeaders(settings, window, "Code::Blocks Project"));
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
	group->addSpace();

	group->addContent(new QtProjectWizardContentPathsHeaderSearch(settings, window, true));
	group->addContent(new QtProjectWizardContentPathsHeaderSearchGlobal(window));
	group->addSpace();

	if (utility::getOsType() == OS_MAC)
	{
		group->addContent(new QtProjectWizardContentPathsFrameworkSearch(settings, window, true));
		group->addContent(new QtProjectWizardContentPathsFrameworkSearchGlobal(window));
		group->addSpace();
	}

	group->addContent(new QtProjectWizardContentFlags(settings, window, true));
}

#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE

template <>
void addSourceGroupContents<SourceGroupSettingsJavaEmpty>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsJavaEmpty> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentJavaStandard(settings, window));
	group->addSpace();
	group->addContent(new QtProjectWizardContentPathsSource(settings, window));
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
	group->addSpace();
	group->addContent(new QtProjectWizardContentPathsClassJava(settings, window));
}

template <>
void addSourceGroupContents<SourceGroupSettingsJavaMaven>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentJavaStandard(settings, window));
	group->addContent(new QtProjectWizardContentPathSourceMaven(settings, window));
	group->addContent(new QtProjectWizardContentPathSettingsMaven(settings, window));
	group->addSpace();
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
}

template <>
void addSourceGroupContents<SourceGroupSettingsJavaGradle>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentJavaStandard(settings, window));
	group->addContent(new QtProjectWizardContentPathSourceGradle(settings, window));
	group->addSpace();
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
}

#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE

template <>
void addSourceGroupContents<SourceGroupSettingsPythonEmpty>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsPythonEmpty> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentPathPythonEnvironment(settings, window));
	group->addContent(new QtProjectWizardContentPathsSource(settings, window));
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
}

#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

template <>
void addSourceGroupContents<SourceGroupSettingsCustomCommand>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsCustomCommand> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentCustomCommand(settings, window));
	group->addContent(new QtProjectWizardContentPathsSource(settings, window));
	group->addContent(new QtProjectWizardContentPathsExclude(settings, window));
	group->addContent(new QtProjectWizardContentExtensions(settings, window));
}

template <>
void addSourceGroupContents<SourceGroupSettingsUnloadable>(
	QtProjectWizardContentGroup* group,
	std::shared_ptr<SourceGroupSettingsUnloadable> settings,
	QtProjectWizardWindow* window)
{
	group->addContent(new QtProjectWizardContentUnloadable(settings, window));
}
}	 // namespace

QtProjectWizard::QtProjectWizard(QWidget* parent)
	: QtProjectWizardWindow(parent, false)
	, m_windowStack(this)
	, m_editing(false)
	, m_previouslySelectedIndex(-1)
	, m_contentWidget(nullptr)
{
	setScrollAble(true);

	connect(&m_windowStack, &QtWindowStack::push, this, &QtProjectWizard::windowStackChanged);
	connect(&m_windowStack, &QtWindowStack::pop, this, &QtProjectWizard::windowStackChanged);
	connect(&m_windowStack, &QtWindowStack::empty, this, &QtProjectWizard::windowStackChanged);

	// save old application settings so they can be compared later
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	m_appSettings.setHeaderSearchPaths(appSettings->getHeaderSearchPaths());
	m_appSettings.setFrameworkSearchPaths(appSettings->getFrameworkSearchPaths());
}

QSize QtProjectWizard::sizeHint() const
{
	return QSize(900, 600);
}

void QtProjectWizard::newProject()
{
	m_projectSettings = std::make_shared<ProjectSettings>();
	setup();
}

void QtProjectWizard::newProjectFromCDB(const FilePath& filePath)
{
#if BUILD_CXX_LANGUAGE_PACKAGE
	if (!m_projectSettings)
	{
		m_projectSettings = std::make_shared<ProjectSettings>();
	}

	if (m_projectSettings->getProjectFilePath().empty())
	{
		m_projectSettings->setProjectFilePath(
			filePath.withoutExtension().fileName(), filePath.getParentDirectory());
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
		std::make_shared<SourceGroupSettingsCxxCdb>(
			utility::getUuidString(), m_projectSettings.get());
	sourceGroupSettings->setCompilationDatabasePath(filePath);

	createSourceGroup(sourceGroupSettings);
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
}

void QtProjectWizard::editProject(const FilePath& settingsPath)
{
	std::shared_ptr<ProjectSettings> settings = std::make_shared<ProjectSettings>(settingsPath);
	settings->reload();
	editProject(settings);
}

void QtProjectWizard::editProject(std::shared_ptr<ProjectSettings> settings)
{
	m_projectSettings = settings;
	m_allSourceGroupSettings = settings->getAllSourceGroupSettings();
	m_editing = true;

	setup();
}

void QtProjectWizard::populateWindow(QWidget* widget)
{
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	{
		QWidget* menu = new QWidget();
		menu->setObjectName(QStringLiteral("project_menu"));
		menu->setFixedWidth(200);
		layout->addWidget(menu);

		QVBoxLayout* menuLayout = new QVBoxLayout(menu);
		menuLayout->setContentsMargins(0, 0, 0, 0);
		menuLayout->setSpacing(0);

		m_generalButton = new QPushButton(QStringLiteral("General"));
		m_generalButton->setObjectName(QStringLiteral("general_button"));
		m_generalButton->setCheckable(true);
		connect(m_generalButton, &QPushButton::clicked, this, &QtProjectWizard::generalButtonClicked);
		menuLayout->addWidget(m_generalButton);

		QLabel* sourceGroupLabel = new QLabel(QStringLiteral("Source Groups:"));
		menuLayout->addWidget(sourceGroupLabel);

		m_sourceGroupList = new QListWidget();
		m_sourceGroupList->setAttribute(Qt::WA_MacShowFocusRect, 0);
		connect(
			m_sourceGroupList,
			&QListWidget::currentRowChanged,
			this,
			&QtProjectWizard::selectedSourceGroupChanged);
		menuLayout->addWidget(m_sourceGroupList);

		QHBoxLayout* buttonsLayout = new QHBoxLayout();
		buttonsLayout->setContentsMargins(0, 0, 0, 0);
		buttonsLayout->setSpacing(0);

		QPushButton* addButton = new QtIconButton(
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_add.png"),
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_add_hover.png"));

		m_removeButton = new QtIconButton(
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_delete.png"),
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_delete_hover.png"));

		m_duplicateButton = new QtIconButton(
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_copy.png"),
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_copy_hover.png"));

		addButton->setIconSize(QSize(20, 20));
		m_removeButton->setIconSize(QSize(20, 20));
		m_duplicateButton->setIconSize(QSize(20, 20));

		addButton->setToolTip(QStringLiteral("Add Source Group"));
		m_removeButton->setToolTip(QStringLiteral("Remove Source Group"));
		m_duplicateButton->setToolTip(QStringLiteral("Copy Source Group"));

		addButton->setObjectName(QStringLiteral("action_button"));
		m_removeButton->setObjectName(QStringLiteral("action_button"));
		m_duplicateButton->setObjectName(QStringLiteral("action_button"));

		m_removeButton->setEnabled(false);
		m_duplicateButton->setEnabled(false);

		connect(addButton, &QPushButton::clicked, this, &QtProjectWizard::newSourceGroup);
		connect(
			m_removeButton, &QPushButton::clicked, this, &QtProjectWizard::removeSelectedSourceGroup);
		connect(
			m_duplicateButton,
			&QPushButton::clicked,
			this,
			&QtProjectWizard::duplicateSelectedSourceGroup);

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
	m_contentWidget->setObjectName(QStringLiteral("form"));

	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setFrameShadow(QFrame::Plain);
	scrollArea->setObjectName(QStringLiteral("formArea"));
	scrollArea->setWidgetResizable(true);

	scrollArea->setWidget(m_contentWidget);
	layout->addWidget(scrollArea);

	widget->setLayout(layout);
}

void QtProjectWizard::windowReady()
{
	if (m_editing)
	{
		updateTitle(QStringLiteral("Edit Project"));
		updateNextButton(QStringLiteral("Save"));
	}
	else
	{
		updateTitle(QStringLiteral("New Project"));
		updateNextButton(QStringLiteral("Create"));
		setNextEnabled(false);
	}

	updateSubTitle(QStringLiteral("Overview"));
	updatePreviousButton(QStringLiteral("Add Source Group"));

	connect(this, &QtProjectWizard::previous, this, &QtProjectWizard::newSourceGroup);
	connect(this, &QtProjectWizard::next, this, &QtProjectWizard::createProject);

	updateSourceGroupList();

	if (!m_allSourceGroupSettings.empty())
	{
		m_sourceGroupList->setCurrentRow(0);
	}
	else
	{
		generalButtonClicked();
	}
}

void QtProjectWizard::handlePrevious()
{
	QtWindow::handlePrevious();
}

QtProjectWizardWindow* QtProjectWizard::createWindowWithContent(
	std::function<QtProjectWizardContent*(QtProjectWizardWindow*)> func)
{
	QtProjectWizardWindow* window = new QtProjectWizardWindow(parentWidget());

	connect(window, &QtProjectWizardWindow::previous, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtProjectWizardWindow::canceled, this, &QtProjectWizard::cancelSourceGroup);

	window->setPreferredSize(QSize(580, 340));
	window->setContent(func(window));
	window->setScrollAble(true);
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

void QtProjectWizard::updateSourceGroupList()
{
	m_sourceGroupList->blockSignals(true);
	m_sourceGroupList->clear();

	for (const std::shared_ptr<SourceGroupSettings>& group: m_allSourceGroupSettings)
	{
		QString name = QString::fromStdString(group->getName());
		if (group->getStatus() == SOURCE_GROUP_STATUS_DISABLED)
		{
			name = "(" + name + ")";
		}

		QListWidgetItem* item = new QListWidgetItem(name);
		m_sourceGroupList->addItem(item);
	}
	m_sourceGroupList->blockSignals(false);
}

bool QtProjectWizard::canExitContent()
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

void QtProjectWizard::generalButtonClicked()
{
	if (!canExitContent())
	{
		m_generalButton->setChecked(false);
		return;
	}

	QtProjectWizardContentGroup* contentGroup = new QtProjectWizardContentGroup(this);
	contentGroup->addContent(
		new QtProjectWizardContentProjectData(m_projectSettings, this, m_editing));

	contentGroup->addSpace();
	contentGroup->addContent(new QtProjectWizardContentRequiredLabel(this));

	if (m_allSourceGroupSettings.empty())
	{
		contentGroup->addSpace();
		contentGroup->addContent(new QtProjectWizardContentSourceGroupInfoText(this));
	}

	setContent(contentGroup);

	qDeleteAll(m_contentWidget->children());
	QtProjectWizardWindow::populateWindow(m_contentWidget);

	loadContent();

	m_previouslySelectedIndex = -1;

	m_generalButton->setChecked(true);
	m_sourceGroupList->setCurrentRow(-1);
}

void QtProjectWizard::selectedSourceGroupChanged(int index)
{
	if (index < 0 || index >= int(m_allSourceGroupSettings.size()))
	{
		m_removeButton->setEnabled(false);
		m_duplicateButton->setEnabled(false);
		return;
	}

	if (index == m_previouslySelectedIndex)
	{
		return;
	}

	if (!canExitContent())
	{
		QTimer::singleShot(1, [&]() { m_sourceGroupList->setCurrentRow(m_previouslySelectedIndex); });
		return;
	}

	m_generalButton->setChecked(false);
	m_removeButton->setEnabled(true);
	m_duplicateButton->setEnabled(true);

	std::shared_ptr<SourceGroupSettings> group = m_allSourceGroupSettings[index];

	QtProjectWizardContentGroup* summary = new QtProjectWizardContentGroup(this);

	QtProjectWizardContentSourceGroupData* content = new QtProjectWizardContentSourceGroupData(
		group, this);
	connect(
		content,
		&QtProjectWizardContentSourceGroupData::nameUpdated,
		this,
		&QtProjectWizard::selectedSourceGroupNameChanged);
	summary->addContent(content);
	summary->addSpace();

	if (std::shared_ptr<SourceGroupSettingsCustomCommand> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCustomCommand>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsUnloadable> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsUnloadable>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
#if BUILD_CXX_LANGUAGE_PACKAGE
	else if (
		std::shared_ptr<SourceGroupSettingsCEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCxxCdb> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	else if (
		std::shared_ptr<SourceGroupSettingsJavaEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsJavaEmpty>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsJavaMaven> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsJavaGradle> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
	else if (
		std::shared_ptr<SourceGroupSettingsPythonEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsPythonEmpty>(group))
	{
		addSourceGroupContents(summary, settings, this);
	}
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

	summary->addSpace();
	summary->addContent(new QtProjectWizardContentRequiredLabel(this));

	setContent(summary);

	qDeleteAll(m_contentWidget->children());
	QtProjectWizardWindow::populateWindow(m_contentWidget);

	loadContent();

	m_previouslySelectedIndex = index;
}

void QtProjectWizard::selectedSourceGroupNameChanged(const QString& name)
{
	m_sourceGroupList->item(m_sourceGroupList->currentRow())->setText(name);
}

void QtProjectWizard::removeSelectedSourceGroup()
{
	if (!m_allSourceGroupSettings.size() || m_sourceGroupList->currentRow() < 0)
	{
		return;
	}

	QMessageBox msgBox(this);
	msgBox.setText(QStringLiteral("Remove Source Group"));
	msgBox.setInformativeText(
		QStringLiteral("Do you really want to remove this source group from the project?"));
	msgBox.addButton(QStringLiteral("Yes"), QMessageBox::ButtonRole::YesRole);
	msgBox.addButton(QStringLiteral("No"), QMessageBox::ButtonRole::NoRole);
	msgBox.setIcon(QMessageBox::Icon::Question);
	int ret = msgBox.exec();

	if (ret == 0)	 // QMessageBox::Yes
	{
		int currentRow = m_sourceGroupList->currentRow();
		m_allSourceGroupSettings.erase(m_allSourceGroupSettings.begin() + currentRow);

		updateSourceGroupList();
		setContent(nullptr);

		if (m_allSourceGroupSettings.empty())
		{
			setNextEnabled(false);
			generalButtonClicked();
			return;
		}

		if (currentRow >= int(m_allSourceGroupSettings.size()))
		{
			currentRow--;
		}

		m_previouslySelectedIndex = -1;

		m_sourceGroupList->setCurrentRow(currentRow);
	}
}

void QtProjectWizard::duplicateSelectedSourceGroup()
{
	if (!m_allSourceGroupSettings.size() || m_sourceGroupList->currentRow() < 0 || !canExitContent())
	{
		return;
	}

	std::shared_ptr<const SourceGroupSettings> oldSourceGroup =
		m_allSourceGroupSettings[m_sourceGroupList->currentRow()];

	std::shared_ptr<SourceGroupSettings> newSourceGroup = oldSourceGroup->createCopy();

	newSourceGroup->setId(utility::getUuidString());

	{
		const std::string newNameBase = oldSourceGroup->getName() + " - Copy";

		int id = 0;
		std::string newName;

		while (true)
		{
			newName = newNameBase + (id > 0 ? " (" + std::to_string(id) + ")" : "");

			bool nameAlreadyExists = false;
			for (const std::shared_ptr<SourceGroupSettings>& sourceGroupSettings:
				 m_allSourceGroupSettings)
			{
				if (sourceGroupSettings && sourceGroupSettings->getName() == newName)
				{
					nameAlreadyExists = true;
					break;
				}
			}

			if (!nameAlreadyExists)
			{
				break;
			}

			id++;
		}

		newSourceGroup->setName(newName);
	}

	const int newRow = m_sourceGroupList->currentRow() + 1;
	m_allSourceGroupSettings.insert(m_allSourceGroupSettings.begin() + newRow, newSourceGroup);

	updateSourceGroupList();
	setContent(nullptr);

	m_previouslySelectedIndex = -1;

	m_sourceGroupList->setCurrentRow(newRow);
}

void QtProjectWizard::cancelSourceGroup()
{
	m_windowStack.clearWindows();
}

void QtProjectWizard::cancelWizard()
{
	m_windowStack.clearWindows();
	emit canceled();
}

void QtProjectWizard::finishWizard()
{
	m_windowStack.clearWindows();
	emit finished();
}

void QtProjectWizard::windowStackChanged()
{
	QWidget* window = m_windowStack.getTopWindow();

	if (window)
	{
		dynamic_cast<QtProjectWizardWindow*>(window)->content()->load();
		setEnabled(false);
	}
	else
	{
		setEnabled(true);
		raise();
	}
}

void QtProjectWizard::newSourceGroup()
{
	if (!canExitContent())
	{
		return;
	}

	QtProjectWizardWindow* window = createWindowWithContent([](QtProjectWizardWindow* window) {
		window->setPreferredSize(QSize(560, 520));
		return new QtProjectWizardContentSelect(window);
	});
	window->resize(QSize(560, 520));

	connect(
		dynamic_cast<QtProjectWizardContentSelect*>(window->content()),
		&QtProjectWizardContentSelect::selected,
		this,
		&QtProjectWizard::selectedProjectType);

	window->show();
	window->setNextEnabled(false);
	window->setPreviousEnabled(false);
	window->updateSubTitle(QStringLiteral("Type Selection"));
}

void QtProjectWizard::newSourceGroupFromVS()
{
#if BUILD_CXX_LANGUAGE_PACKAGE
	QtProjectWizardWindow* window = createWindowWithContent([](QtProjectWizardWindow* window) {
		window->setPreferredSize(QSize(560, 320));
		return new QtProjectWizardContentVS(window);
	});
	window->resize(QSize(560, 320));

	connect(window, &QtProjectWizardWindow::next, [this]() {
		selectedProjectType(SOURCE_GROUP_CXX_CDB);
	});

	window->show();
	window->setNextEnabled(true);
	window->setPreviousEnabled(true);
	window->updateSubTitle(QStringLiteral("C/C++ from Visual Studio"));
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
}

void QtProjectWizard::selectedProjectType(SourceGroupType sourceGroupType)
{
	const std::string sourceGroupId = utility::getUuidString();
	std::shared_ptr<SourceGroupSettings> settings;

	switch (sourceGroupType)
	{
#if BUILD_CXX_LANGUAGE_PACKAGE
	case SOURCE_GROUP_C_EMPTY:
	{
		std::shared_ptr<SourceGroupSettingsCEmpty> cxxSettings =
			std::make_shared<SourceGroupSettingsCEmpty>(sourceGroupId, m_projectSettings.get());
		addMsvcCompatibilityFlagsOnDemand(cxxSettings);
		settings = cxxSettings;
	}
	break;
	case SOURCE_GROUP_CPP_EMPTY:
	{
		std::shared_ptr<SourceGroupSettingsCppEmpty> cxxSettings =
			std::make_shared<SourceGroupSettingsCppEmpty>(sourceGroupId, m_projectSettings.get());
		addMsvcCompatibilityFlagsOnDemand(cxxSettings);
		settings = cxxSettings;
	}
	break;
	case SOURCE_GROUP_CXX_CDB:
		settings = std::make_shared<SourceGroupSettingsCxxCdb>(
			sourceGroupId, m_projectSettings.get());
		break;
	case SOURCE_GROUP_CXX_CODEBLOCKS:
	{
		std::shared_ptr<SourceGroupSettingsCxxCodeblocks> cxxSettings =
			std::make_shared<SourceGroupSettingsCxxCodeblocks>(
				sourceGroupId, m_projectSettings.get());
		addMsvcCompatibilityFlagsOnDemand(cxxSettings);
		settings = cxxSettings;
	}
	break;
	case SOURCE_GROUP_CXX_VS:
		newSourceGroupFromVS();
		return;
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
	case SOURCE_GROUP_JAVA_EMPTY:
		settings = std::make_shared<SourceGroupSettingsJavaEmpty>(
			sourceGroupId, m_projectSettings.get());
		break;
	case SOURCE_GROUP_JAVA_MAVEN:
		settings = std::make_shared<SourceGroupSettingsJavaMaven>(
			sourceGroupId, m_projectSettings.get());
		break;
	case SOURCE_GROUP_JAVA_GRADLE:
		settings = std::make_shared<SourceGroupSettingsJavaGradle>(
			sourceGroupId, m_projectSettings.get());
		break;
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

#if BUILD_PYTHON_LANGUAGE_PACKAGE
	case SOURCE_GROUP_PYTHON_EMPTY:
		settings = std::make_shared<SourceGroupSettingsPythonEmpty>(
			sourceGroupId, m_projectSettings.get());
		break;
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

	case SOURCE_GROUP_CUSTOM_COMMAND:
		settings = std::make_shared<SourceGroupSettingsCustomCommand>(
			sourceGroupId, m_projectSettings.get());
		break;
	case SOURCE_GROUP_UNKNOWN:
		break;
	}

	if (settings)
	{
		createSourceGroup(settings);
	}
}

void QtProjectWizard::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	m_allSourceGroupSettings.push_back(settings);

	updateSourceGroupList();
	cancelSourceGroup();

	setNextEnabled(true);

	m_previouslySelectedIndex = -1;

	m_sourceGroupList->setCurrentRow(static_cast<int>(m_allSourceGroupSettings.size()) - 1);
}

void QtProjectWizard::createProject()
{
	FilePath path = m_projectSettings->getFilePath();

	m_projectSettings->setVersion(ProjectSettings::VERSION);
	m_projectSettings->setAllSourceGroupSettings(m_allSourceGroupSettings);
	if (!m_projectSettings->save(path))
	{
		MessageStatus(L"Unable to save project to location: " + path.wstr()).dispatch();

		QMessageBox msgBox(this);
		msgBox.setText(QStringLiteral("Could not create Project"));
		msgBox.setInformativeText(QString::fromStdWString(
			L"<p>Sourcetrail was unable to save the project to the specified path. Please pick a "
			L"different project location.</p>"));
		msgBox.addButton(QStringLiteral("Ok"), QMessageBox::ButtonRole::AcceptRole);
		msgBox.exec();

		return;
	}

	bool settingsChanged = false;
	if (m_editing)
	{
		std::shared_ptr<const Project> currentProject =
			Application::getInstance()->getCurrentProject();
		if (currentProject)
		{
			settingsChanged = !(
				currentProject->settingsEqualExceptNameAndLocation(*(m_projectSettings.get())));
		}

		settingsChanged |= !(m_appSettings == *ApplicationSettings::getInstance().get());
	}
	else
	{
		MessageStatus(L"Created project: " + path.wstr()).dispatch();
	}

	MessageLoadProject(path, settingsChanged).dispatch();

	finishWizard();
}
