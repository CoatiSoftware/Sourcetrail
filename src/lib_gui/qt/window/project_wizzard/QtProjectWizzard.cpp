#include "qt/window/project_wizzard/QtProjectWizzard.h"

#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QScrollArea>
#include <QSysInfo>
#include <QTimer>

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCppStandard.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCrossCompilationOptions.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentCStandard.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentExtensions.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentJavaStandard.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPath.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentProjectData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSourceGroupData.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSourceGroupInfoText.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentGroup.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentVS.h"
#include "qt/window/project_wizzard/QtSourceGroupWizzardPage.h"
#include "settings/SourceGroupSettingsCEmpty.h"
#include "settings/SourceGroupSettingsCppEmpty.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "settings/SourceGroupSettingsCxxCdbVs.h"
#include "settings/SourceGroupSettingsCxxCodeblocks.h"
#include "settings/SourceGroupSettingsCxxSonargraph.h"
#include "settings/SourceGroupSettingsJavaEmpty.h"
#include "settings/SourceGroupSettingsJavaGradle.h"
#include "settings/SourceGroupSettingsJavaMaven.h"
#include "settings/SourceGroupSettingsJavaSonargraph.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/utility.h"
#include "utility/utilityApp.h"
#include "utility/utilityPathDetection.h"
#include "utility/utilityString.h"
#include "utility/utilityUuid.h"

#include "Application.h"

namespace
{
	bool applicationSettingsContainVisualStudioHeaderSearchPaths()
	{
		std::vector<FilePath> expandedPaths;
		const std::shared_ptr<CombinedPathDetector> headerPathDetector = utility::getCxxVsHeaderPathDetector();
		for (const std::string& detectorName : headerPathDetector->getWorkingDetectorNames())
		{
			for (const FilePath& path : headerPathDetector->getPaths(detectorName))
			{
				utility::append(expandedPaths, path.expandEnvironmentVariables());
			}
		}

		std::vector<FilePath> usedExpandedGlobalHeaderSearchPaths =
			ApplicationSettings::getInstance()->getHeaderSearchPathsExpanded();
		for (const FilePath& usedExpandedPath : usedExpandedGlobalHeaderSearchPaths)
		{
			for (const FilePath& expandedPath : expandedPaths)
			{
				if (expandedPath == usedExpandedPath)
				{
					return true;
				}
			}
		}

		return false;
	}

	void addMsvcCompatibilityFlagsOnDemand(std::shared_ptr<SourceGroupSettingsCxx> settings)
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

	template <typename SettingsType>
	std::vector<QtSourceGroupWizzardPage<SettingsType>> getSourceGroupWizzardPages();

	template <>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCEmpty>> getSourceGroupWizzardPages<SourceGroupSettingsCEmpty>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCEmpty>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCEmpty> page("Language Standard", 470, 460);
			page.addContentCreatorWithSettings<QtProjectWizzardContentCStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentCrossCompilationOptions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCEmpty> page("Indexed Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsSource>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentExtensions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCEmpty> page("Include Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsHeaderSearch>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsHeaderSearchGlobal>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		if (utility::getOsType() == OS_MAC)
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCEmpty> page("Framework Search Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsFrameworkSearch>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsFrameworkSearchGlobal>(WIZZARD_CONTENT_CONTEXT_ALL); 
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCEmpty> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentFlags>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}

		return pages;
	}

	template <>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCppEmpty>> getSourceGroupWizzardPages<SourceGroupSettingsCppEmpty>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCppEmpty>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCppEmpty> page("Language Standard", 470, 460);
			page.addContentCreatorWithSettings<QtProjectWizzardContentCppStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentCrossCompilationOptions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCppEmpty> page("Indexed Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsSource>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentExtensions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCppEmpty> page("Include Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsHeaderSearch>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsHeaderSearchGlobal>(WIZZARD_CONTENT_CONTEXT_ALL); 
			pages.push_back(page);
		}
		if (utility::getOsType() == OS_MAC)
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCppEmpty> page("Framework Search Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsFrameworkSearch>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsFrameworkSearchGlobal>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCppEmpty> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentFlags>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}

		return pages;
	}

	template <>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdb>> getSourceGroupWizzardPages<SourceGroupSettingsCxxCdb>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdb>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdb> page("Compilation Database Path");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathCDB>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentIndexedHeaderPaths>(WIZZARD_CONTENT_CONTEXT_ALL, "Compilation Database");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdb> page("Include Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsHeaderSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsHeaderSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		if (utility::getOsType() == OS_MAC)
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdb> page("Framework Search Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsFrameworkSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsFrameworkSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdb> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentFlags>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			pages.push_back(page);
		}

		return pages;
	}

	template <>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxCodeblocks>> getSourceGroupWizzardPages<SourceGroupSettingsCxxCodeblocks>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxCodeblocks>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCodeblocks> page("Code::Blocks Project Path");
			page.addContentCreatorWithSettings<QtProjectWizzardContentCppStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentCStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentCodeblocksProjectPath>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentIndexedHeaderPaths>(WIZZARD_CONTENT_CONTEXT_ALL, std::string("Code::Blocks Project"));
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentExtensions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCodeblocks> page("Include Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsHeaderSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsHeaderSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		if (utility::getOsType() == OS_MAC)
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCodeblocks> page("Framework Search Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsFrameworkSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsFrameworkSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCodeblocks> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentFlags>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			pages.push_back(page);
		}

		return pages;
	}

	template <>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxSonargraph>> getSourceGroupWizzardPages<SourceGroupSettingsCxxSonargraph>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxSonargraph>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxSonargraph> page("Sonargraph Project Path");
			page.addContentCreatorWithSettings<QtProjectWizzardContentCppStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreator(
				WIZZARD_CONTENT_CONTEXT_ALL,
				[](std::shared_ptr<SourceGroupSettingsCxxSonargraph> settings, QtProjectWizzardWindow* window)
				{
					return new QtProjectWizzardContentSonargraphProjectPath(settings, settings, settings, window);
				}
			);
			page.addContentCreatorWithSettings<QtProjectWizzardContentIndexedHeaderPaths>(WIZZARD_CONTENT_CONTEXT_ALL, std::string("Sonargraph project"));
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxSonargraph> page("Include Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsHeaderSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsHeaderSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		if (utility::getOsType() == OS_MAC)
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxSonargraph> page("Framework Search Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsFrameworkSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsFrameworkSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxSonargraph> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentFlags>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			pages.push_back(page);
		}

		return pages;
	}

	template <>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdbVs>> getSourceGroupWizzardPages<SourceGroupSettingsCxxCdbVs>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdbVs>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdbVs> page("VS Solution");
			page.addContentCreatorSimple<QtProjectWizzardContentVS>(WIZZARD_CONTENT_CONTEXT_SETUP);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathCDB>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentIndexedHeaderPaths>(WIZZARD_CONTENT_CONTEXT_ALL, std::string("Compilation Database"));
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdbVs> page("Include Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsHeaderSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsHeaderSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		if (utility::getOsType() == OS_MAC)
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdbVs> page("Framework Search Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsFrameworkSearch>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			page.addContentCreatorSimple<QtProjectWizzardContentPathsFrameworkSearchGlobal>(WIZZARD_CONTENT_CONTEXT_SUMMARY);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsCxxCdbVs> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentFlags>(WIZZARD_CONTENT_CONTEXT_SUMMARY, true);
			pages.push_back(page);
		}

		return pages;
	}


	template<>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaEmpty>> getSourceGroupWizzardPages<SourceGroupSettingsJavaEmpty>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaEmpty>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaEmpty> page("Language Standard", 470, 230);
			page.addContentCreatorWithSettings<QtProjectWizzardContentJavaStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaEmpty> page("Indexed Paths", 750, 600);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsSource>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentExtensions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaEmpty> page("Dependencies", 750, 600);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsClassJava>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}

		return pages;
	}

	template<>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaMaven>> getSourceGroupWizzardPages<SourceGroupSettingsJavaMaven>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaMaven>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaMaven> page("Indexed Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentJavaStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathSourceMaven>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathDependenciesMaven>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaMaven> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentExtensions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}

		return pages;
	}

	template<>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaGradle>> getSourceGroupWizzardPages<SourceGroupSettingsJavaGradle>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaGradle>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaGradle> page("Indexed Paths");
			page.addContentCreatorWithSettings<QtProjectWizzardContentJavaStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathSourceGradle>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathDependenciesGradle>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaGradle> page("Advanced (optional)");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsExclude>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreatorWithSettings<QtProjectWizzardContentExtensions>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}

		return pages;
	}

	template<>
	std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaSonargraph>> getSourceGroupWizzardPages<SourceGroupSettingsJavaSonargraph>()
	{
		std::vector<QtSourceGroupWizzardPage<SourceGroupSettingsJavaSonargraph>> pages;
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaSonargraph> page("Sonargraph Project Path");
			page.addContentCreatorWithSettings<QtProjectWizzardContentJavaStandard>(WIZZARD_CONTENT_CONTEXT_ALL);
			page.addContentCreator(
				WIZZARD_CONTENT_CONTEXT_ALL,
				[](std::shared_ptr<SourceGroupSettingsJavaSonargraph> settings, QtProjectWizzardWindow* window)
				{
					return new QtProjectWizzardContentSonargraphProjectPath(settings, std::shared_ptr<SourceGroupSettingsCxxSonargraph>(), settings, window);
				}
			);
			pages.push_back(page);
		}
		{
			QtSourceGroupWizzardPage<SourceGroupSettingsJavaSonargraph> page("Dependencies");
			page.addContentCreatorWithSettings<QtProjectWizzardContentPathsClassJava>(WIZZARD_CONTENT_CONTEXT_ALL);
			pages.push_back(page);
		}

		return pages;
	}

	template <typename SettingsType>
	void fillSummary(QtProjectWizzardContentGroup* summary, std::shared_ptr<SettingsType> settings, QtProjectWizzardWindow* window)
	{
		const std::vector<QtSourceGroupWizzardPage<SettingsType>> pages = getSourceGroupWizzardPages<SettingsType>();

		for (const QtSourceGroupWizzardPage<SettingsType>& page : pages)
		{
			QtProjectWizzardContentGroup* contentGroup = page.createContentGroup(WIZZARD_CONTENT_CONTEXT_SUMMARY, settings, window);
			if (contentGroup && contentGroup->hasContents())
			{
				summary->addContent(contentGroup);
				summary->addSpace();
			}
		}
	}
}

QtProjectWizzard::QtProjectWizzard(QWidget* parent)
	: QtProjectWizzardWindow(parent, false)
	, m_windowStack(this)
	, m_editing(false)
	, m_previouslySelectedIndex(-1)
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
		m_projectSettings->setProjectFilePath(filePath.withoutExtension().fileName(), filePath.getParentDirectory());
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

	std::shared_ptr<SourceGroupSettingsCxxCdbVs> sourceGroupSettings =
		std::make_shared<SourceGroupSettingsCxxCdbVs>(utility::getUuidString(), m_projectSettings.get());
	sourceGroupSettings->setCompilationDatabasePath(filePath);

	executeSoureGroupSetup<SourceGroupSettingsCxxCdbVs>(sourceGroupSettings);
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
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_add.png"),
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_add_hover.png")
		);

		m_removeButton = new QtIconButton(
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_delete.png"),
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_delete_hover.png")
		);

		m_duplicateButton = new QtIconButton(
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_copy.png"),
			ResourcePaths::getGuiPath().concatenate(L"window/source_group_copy_hover.png")
		);

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

	if (!m_allSourceGroupSettings.empty())
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

template <typename SettingsType>
void QtProjectWizzard::executeSoureGroupSetup(std::shared_ptr<SettingsType> settings)
{
	std::shared_ptr<QtSourceGroupWizzard<SettingsType>> wizzard = std::make_shared<QtSourceGroupWizzard<SettingsType>>(
		settings,
		std::bind(&QtProjectWizzard::cancelSourceGroup, this),
		std::bind(&QtProjectWizzard::createSourceGroup, this, std::placeholders::_1)
		);

	for (const QtSourceGroupWizzardPage<SettingsType>& page : getSourceGroupWizzardPages<SettingsType>())
	{
		wizzard->addPage(page);
	}

	m_sourceGroupWizzard = wizzard;
	m_sourceGroupWizzard->execute(m_windowStack);
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

	m_previouslySelectedIndex = -1;

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

	QtProjectWizzardContentGroup* summary = new QtProjectWizzardContentGroup(this);
	summary->setIsForm(true);

	QtProjectWizzardContentSourceGroupData* content = new QtProjectWizzardContentSourceGroupData(group, this);
	connect(content, &QtProjectWizzardContentSourceGroupData::nameUpdated, this, &QtProjectWizzard::selectedSourceGroupNameChanged);
	summary->addContent(content);
	summary->addSpace();

	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings = std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings = std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsCxxCdb> settings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsCxxSonargraph> settings = std::dynamic_pointer_cast<SourceGroupSettingsCxxSonargraph>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsCxxCdbVs> settings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdbVs>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaEmpty> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaEmpty>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (std::shared_ptr<SourceGroupSettingsJavaSonargraph> settings = std::dynamic_pointer_cast<SourceGroupSettingsJavaSonargraph>(group))
	{
		fillSummary(summary, settings, this);
	}

	setContent(summary);

	qDeleteAll(m_contentWidget->children());
	QtProjectWizzardWindow::populateWindow(m_contentWidget);

	loadContent();

	m_previouslySelectedIndex = index;
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

void QtProjectWizzard::duplicateSelectedSourceGroup()
{
	if (!m_allSourceGroupSettings.size() || m_sourceGroupList->currentRow() < 0 || !canExitContent())
	{
		return;
	}

	std::shared_ptr<const SourceGroupSettings> oldSourceGroup = m_allSourceGroupSettings[m_sourceGroupList->currentRow()];

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
			for (std::shared_ptr<SourceGroupSettings> sourceGroupSettings : m_allSourceGroupSettings)
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

	m_previouslySelectedIndex = -1;

	m_sourceGroupList->setCurrentRow(newRow);
}

void QtProjectWizzard::cancelSourceGroup()
{
	m_windowStack.clearWindows();
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
		[](QtProjectWizzardWindow* window)
		{
			window->setPreferredSize(QSize(560, 520));
			return new QtProjectWizzardContentSelect(window);
		}
	);
	window->resize(QSize(560, 520));

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
		{
			std::shared_ptr<SourceGroupSettingsCEmpty> settings = std::make_shared<SourceGroupSettingsCEmpty>(sourceGroupId, m_projectSettings.get());
			addMsvcCompatibilityFlagsOnDemand(settings);
			executeSoureGroupSetup<SourceGroupSettingsCEmpty>(settings);
		}
		break;
	case SOURCE_GROUP_CPP_EMPTY:
	{
		std::shared_ptr<SourceGroupSettingsCppEmpty> settings = std::make_shared<SourceGroupSettingsCppEmpty>(sourceGroupId, m_projectSettings.get());
		addMsvcCompatibilityFlagsOnDemand(settings);
		executeSoureGroupSetup<SourceGroupSettingsCppEmpty>(settings);
	}
		break;
	case SOURCE_GROUP_CXX_CDB:
		{
			std::shared_ptr<SourceGroupSettingsCxxCdb> settings = std::make_shared<SourceGroupSettingsCxxCdb>(sourceGroupId, m_projectSettings.get());
			executeSoureGroupSetup<SourceGroupSettingsCxxCdb>(settings);
		}
		break;
	case SOURCE_GROUP_CXX_CODEBLOCKS:
		{
			std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings = std::make_shared<SourceGroupSettingsCxxCodeblocks>(sourceGroupId, m_projectSettings.get());
			addMsvcCompatibilityFlagsOnDemand(settings);
			executeSoureGroupSetup<SourceGroupSettingsCxxCodeblocks>(settings);
		}
		break;
	case SOURCE_GROUP_CXX_SONARGRAPH:
		{
			std::shared_ptr<SourceGroupSettingsCxxSonargraph> settings = std::make_shared<SourceGroupSettingsCxxSonargraph>(sourceGroupId, m_projectSettings.get());
			addMsvcCompatibilityFlagsOnDemand(settings);
			executeSoureGroupSetup<SourceGroupSettingsCxxSonargraph>(settings);
		}
		break;
	case SOURCE_GROUP_CXX_VS:
		{
			std::shared_ptr<SourceGroupSettingsCxxCdbVs> settings = std::make_shared<SourceGroupSettingsCxxCdbVs>(sourceGroupId, m_projectSettings.get());
			executeSoureGroupSetup<SourceGroupSettingsCxxCdbVs>(settings);
		}
		break;
	case SOURCE_GROUP_JAVA_EMPTY:
		{
			std::shared_ptr<SourceGroupSettingsJavaEmpty> settings = std::make_shared<SourceGroupSettingsJavaEmpty>(sourceGroupId, m_projectSettings.get());
			executeSoureGroupSetup<SourceGroupSettingsJavaEmpty>(settings);
		}
		break;
	case SOURCE_GROUP_JAVA_MAVEN:
		{
			std::shared_ptr<SourceGroupSettingsJavaMaven> settings = std::make_shared<SourceGroupSettingsJavaMaven>(sourceGroupId, m_projectSettings.get());
			settings->setMavenDependenciesDirectory(
				FilePath(
					L"./sourcetrail_dependencies/" + utility::replace(m_projectSettings->getProjectName(), L" ", L"_") +
					L"/" +
					utility::decodeFromUtf8(settings->getId()) +
					L"/maven"
				)
			);
			executeSoureGroupSetup<SourceGroupSettingsJavaMaven>(settings);
		}
		break;
	case SOURCE_GROUP_JAVA_GRADLE:
		{
			std::shared_ptr<SourceGroupSettingsJavaGradle> settings = std::make_shared<SourceGroupSettingsJavaGradle>(sourceGroupId, m_projectSettings.get());
			settings->setGradleDependenciesDirectory(
				FilePath(
					L"./sourcetrail_dependencies/" + utility::replace(m_projectSettings->getProjectName(), L" ", L"_") +
					L"/" +
					utility::decodeFromUtf8(settings->getId()) +
					L"/gradle"
				)
			);
			executeSoureGroupSetup<SourceGroupSettingsJavaGradle>(settings);
		}
		break;
	case SOURCE_GROUP_JAVA_SONARGRAPH: 
		{
			std::shared_ptr<SourceGroupSettingsJavaSonargraph> settings = std::make_shared<SourceGroupSettingsJavaSonargraph>(sourceGroupId, m_projectSettings.get());
			executeSoureGroupSetup<SourceGroupSettingsJavaSonargraph>(settings);
		}
		break;
	case SOURCE_GROUP_UNKNOWN:
		break;
	}
}

void QtProjectWizzard::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	m_allSourceGroupSettings.push_back(settings);

	updateSourceGroupList();
	cancelSourceGroup();

	setNextEnabled(true);

	m_previouslySelectedIndex = -1;

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
		if (application->getCurrentProject() != nullptr)
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
