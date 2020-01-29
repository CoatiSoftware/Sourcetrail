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
#include "QtProjectWizardContentPath.h"
#include "QtProjectWizardContentPaths.h"
#include "QtProjectWizardContentPathsExclude.h"
#include "QtProjectWizardContentPathsSource.h"
#include "QtProjectWizardContentProjectData.h"
#include "QtProjectWizardContentSelect.h"
#include "QtProjectWizardContentSourceGroupData.h"
#include "QtProjectWizardContentSourceGroupInfoText.h"
#include "QtProjectWizardContentUnloadable.h"
#include "QtSourceGroupWizardPage.h"
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
#	include "SourceGroupSettingsCxxCdbVs.h"
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
std::vector<QtSourceGroupWizardPage<SettingsType>> getSourceGroupWizardPages();

#if BUILD_CXX_LANGUAGE_PACKAGE

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCEmpty>> getSourceGroupWizardPages<
	SourceGroupSettingsCEmpty>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCEmpty>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCEmpty> page("Language Standard", 470, 460);
		page.addContentCreatorWithSettings<QtProjectWizardContentCStandard>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentCrossCompilationOptions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCEmpty> page("Indexed Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsSource>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCEmpty> page("Include Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsHeaderSearch>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorSimple<QtProjectWizardContentPathsHeaderSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	if (utility::getOsType() == OS_MAC)
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCEmpty> page("Framework Search Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsFrameworkSearch>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorSimple<QtProjectWizardContentPathsFrameworkSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCEmpty> page("Advanced (optional)");
		page.addContentCreatorWithSettings<QtProjectWizardContentFlags>(WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreator(
			WIZARD_CONTENT_CONTEXT_ALL,
			[](std::shared_ptr<SourceGroupSettingsCEmpty> settings, QtProjectWizardWindow* window) {
				return new QtProjectWizardContentPathCxxPch(settings, settings, window);
			});
		page.addContentCreatorWithSettings<QtProjectWizardContentCxxPchFlags>(
			WIZARD_CONTENT_CONTEXT_ALL, false);
		pages.push_back(page);
	}

	return pages;
}

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCppEmpty>> getSourceGroupWizardPages<
	SourceGroupSettingsCppEmpty>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCppEmpty>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCppEmpty> page("Language Standard", 470, 460);
		page.addContentCreatorWithSettings<QtProjectWizardContentCppStandard>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentCrossCompilationOptions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCppEmpty> page("Indexed Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsSource>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCppEmpty> page("Include Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsHeaderSearch>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorSimple<QtProjectWizardContentPathsHeaderSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	if (utility::getOsType() == OS_MAC)
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCppEmpty> page("Framework Search Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsFrameworkSearch>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorSimple<QtProjectWizardContentPathsFrameworkSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCppEmpty> page("Advanced (optional)");
		page.addContentCreatorWithSettings<QtProjectWizardContentFlags>(WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreator(
			WIZARD_CONTENT_CONTEXT_ALL,
			[](std::shared_ptr<SourceGroupSettingsCppEmpty> settings, QtProjectWizardWindow* window) {
				return new QtProjectWizardContentPathCxxPch(settings, settings, window);
			});
		page.addContentCreatorWithSettings<QtProjectWizardContentCxxPchFlags>(
			WIZARD_CONTENT_CONTEXT_ALL, false);
		pages.push_back(page);
	}

	return pages;
}

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCxxCdb>> getSourceGroupWizardPages<
	SourceGroupSettingsCxxCdb>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCxxCdb>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdb> page("Compilation Database Path");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathCDB>(WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsIndexedHeaders>(
			WIZARD_CONTENT_CONTEXT_ALL, "Compilation Database");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdb> page("Include Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsHeaderSearch>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		page.addContentCreatorSimple<QtProjectWizardContentPathsHeaderSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_SUMMARY);
		pages.push_back(page);
	}
	if (utility::getOsType() == OS_MAC)
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdb> page("Framework Search Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsFrameworkSearch>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		page.addContentCreatorSimple<QtProjectWizardContentPathsFrameworkSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_SUMMARY);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdb> page("Advanced (optional)");
		page.addContentCreatorWithSettings<QtProjectWizardContentFlags>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		page.addContentCreator(
			WIZARD_CONTENT_CONTEXT_ALL,
			[](std::shared_ptr<SourceGroupSettingsCxxCdb> settings, QtProjectWizardWindow* window) {
				return new QtProjectWizardContentPathCxxPch(settings, settings, window);
			});
		page.addContentCreatorWithSettings<QtProjectWizardContentCxxPchFlags>(
			WIZARD_CONTENT_CONTEXT_ALL, true);
		pages.push_back(page);
	}

	return pages;
}

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCxxCodeblocks>> getSourceGroupWizardPages<
	SourceGroupSettingsCxxCodeblocks>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCxxCodeblocks>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCodeblocks> page("Code::Blocks Project Path");
		page.addContentCreatorWithSettings<QtProjectWizardContentCppStandard>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentCStandard>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathCodeblocksProject>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsIndexedHeaders>(
			WIZARD_CONTENT_CONTEXT_ALL, std::string("Code::Blocks Project"));
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCodeblocks> page("Include Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsHeaderSearch>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		page.addContentCreatorSimple<QtProjectWizardContentPathsHeaderSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_SUMMARY);
		pages.push_back(page);
	}
	if (utility::getOsType() == OS_MAC)
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCodeblocks> page("Framework Search Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsFrameworkSearch>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		page.addContentCreatorSimple<QtProjectWizardContentPathsFrameworkSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_SUMMARY);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCodeblocks> page("Advanced (optional)");
		page.addContentCreatorWithSettings<QtProjectWizardContentFlags>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		pages.push_back(page);
	}

	return pages;
}

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCxxCdbVs>> getSourceGroupWizardPages<
	SourceGroupSettingsCxxCdbVs>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCxxCdbVs>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdbVs> page("VS Solution");
		page.addContentCreatorSimple<QtProjectWizardContentVS>(WIZARD_CONTENT_CONTEXT_SETUP);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathCDB>(WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsIndexedHeaders>(
			WIZARD_CONTENT_CONTEXT_ALL, std::string("Compilation Database"));
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdbVs> page("Include Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsHeaderSearch>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		page.addContentCreatorSimple<QtProjectWizardContentPathsHeaderSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_SUMMARY);
		pages.push_back(page);
	}
	if (utility::getOsType() == OS_MAC)
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdbVs> page("Framework Search Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsFrameworkSearch>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		page.addContentCreatorSimple<QtProjectWizardContentPathsFrameworkSearchGlobal>(
			WIZARD_CONTENT_CONTEXT_SUMMARY);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCxxCdbVs> page("Advanced (optional)");
		page.addContentCreatorWithSettings<QtProjectWizardContentFlags>(
			WIZARD_CONTENT_CONTEXT_SUMMARY, true);
		pages.push_back(page);
	}

	return pages;
}

#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsJavaEmpty>> getSourceGroupWizardPages<
	SourceGroupSettingsJavaEmpty>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsJavaEmpty>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsJavaEmpty> page("Language Standard", 470, 230);
		page.addContentCreatorWithSettings<QtProjectWizardContentJavaStandard>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsJavaEmpty> page("Indexed Paths", 750, 600);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsSource>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsJavaEmpty> page("Dependencies", 750, 600);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsClassJava>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}

	return pages;
}

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsJavaMaven>> getSourceGroupWizardPages<
	SourceGroupSettingsJavaMaven>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsJavaMaven>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsJavaMaven> page("Indexed Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentJavaStandard>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathSourceMaven>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathSettingsMaven>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsJavaMaven> page("Advanced (optional)");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}

	return pages;
}

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsJavaGradle>> getSourceGroupWizardPages<
	SourceGroupSettingsJavaGradle>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsJavaGradle>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsJavaGradle> page("Indexed Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentJavaStandard>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathSourceGradle>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	{
		QtSourceGroupWizardPage<SourceGroupSettingsJavaGradle> page("Advanced (optional)");
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}

	return pages;
}

#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsPythonEmpty>> getSourceGroupWizardPages<
	SourceGroupSettingsPythonEmpty>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsPythonEmpty>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsPythonEmpty> page("Indexed Paths", 750, 600);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathPythonEnvironment>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsSource>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}

	return pages;
}

#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCustomCommand>> getSourceGroupWizardPages<
	SourceGroupSettingsCustomCommand>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsCustomCommand>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsCustomCommand> page("Indexed Paths");
		page.addContentCreatorWithSettings<QtProjectWizardContentCustomCommand>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsSource>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentPathsExclude>(
			WIZARD_CONTENT_CONTEXT_ALL);
		page.addContentCreatorWithSettings<QtProjectWizardContentExtensions>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	return pages;
}

template <>
std::vector<QtSourceGroupWizardPage<SourceGroupSettingsUnloadable>> getSourceGroupWizardPages<
	SourceGroupSettingsUnloadable>()
{
	std::vector<QtSourceGroupWizardPage<SourceGroupSettingsUnloadable>> pages;
	{
		QtSourceGroupWizardPage<SourceGroupSettingsUnloadable> page("");
		page.addContentCreatorWithSettings<QtProjectWizardContentUnloadable>(
			WIZARD_CONTENT_CONTEXT_ALL);
		pages.push_back(page);
	}
	return pages;
}

template <typename SettingsType>
void fillSummary(
	QtProjectWizardContentGroup* summary,
	std::shared_ptr<SettingsType> settings,
	QtProjectWizardWindow* window)
{
	const std::vector<QtSourceGroupWizardPage<SettingsType>> pages =
		getSourceGroupWizardPages<SettingsType>();

	for (const QtSourceGroupWizardPage<SettingsType>& page: pages)
	{
		QtProjectWizardContentGroup* contentGroup = page.createContentGroup(
			WIZARD_CONTENT_CONTEXT_SUMMARY, settings, window);
		if (contentGroup && contentGroup->hasContents())
		{
			summary->addContent(contentGroup);
			summary->addSpace();
		}
	}
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

	std::shared_ptr<SourceGroupSettingsCxxCdbVs> sourceGroupSettings =
		std::make_shared<SourceGroupSettingsCxxCdbVs>(
			utility::getUuidString(), m_projectSettings.get());
	sourceGroupSettings->setCompilationDatabasePath(filePath);

	executeSourceGroupSetup<SourceGroupSettingsCxxCdbVs>(sourceGroupSettings);
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

template <typename SettingsType>
void QtProjectWizard::executeSourceGroupSetup(std::shared_ptr<SettingsType> settings)
{
	std::shared_ptr<QtSourceGroupWizard<SettingsType>> wizard =
		std::make_shared<QtSourceGroupWizard<SettingsType>>(
			settings,
			std::bind(&QtProjectWizard::cancelSourceGroup, this),
			std::bind(&QtProjectWizard::createSourceGroup, this, std::placeholders::_1));

	for (const QtSourceGroupWizardPage<SettingsType>& page: getSourceGroupWizardPages<SettingsType>())
	{
		wizard->addPage(page);
	}

	m_sourceGroupWizard = wizard;
	m_sourceGroupWizard->execute(m_windowStack);
}

QtProjectWizardWindow* QtProjectWizard::createWindowWithContent(
	std::function<QtProjectWizardContent*(QtProjectWizardWindow*)> func)
{
	QtProjectWizardWindow* window = new QtProjectWizardWindow(parentWidget());

	connect(window, &QtProjectWizardWindow::previous, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtProjectWizardWindow::canceled, this, &QtProjectWizard::cancelSourceGroup);

	window->setPreferredSize(QSize(580, 340));
	window->setContent(func(window));
	window->setScrollAble(window->content()->isScrollAble());
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

QtProjectWizardWindow* QtProjectWizard::createWindowWithContentGroup(
	std::function<void(QtProjectWizardWindow*, QtProjectWizardContentGroup*)> func)
{
	QtProjectWizardWindow* window = new QtProjectWizardWindow(parentWidget());

	connect(window, &QtProjectWizardWindow::previous, &m_windowStack, &QtWindowStack::popWindow);
	connect(window, &QtProjectWizardWindow::canceled, this, &QtProjectWizard::cancelSourceGroup);

	QtProjectWizardContentGroup* contentGroup = new QtProjectWizardContentGroup(window);

	window->setPreferredSize(QSize(750, 600));
	window->setContent(contentGroup);
	window->setScrollAble(window->content()->isScrollAble());
	func(window, contentGroup);
	window->setup();

	m_windowStack.pushWindow(window);

	return window;
}

void QtProjectWizard::updateSourceGroupList()
{
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
	summary->setIsForm(true);

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
		fillSummary(summary, settings, this);
	}
	if (std::shared_ptr<SourceGroupSettingsUnloadable> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsUnloadable>(group))
	{
		fillSummary(summary, settings, this);
	}
#if BUILD_CXX_LANGUAGE_PACKAGE
	else if (
		std::shared_ptr<SourceGroupSettingsCEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCxxCdb> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCodeblocks>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsCxxCdbVs> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCxxCdbVs>(group))
	{
		fillSummary(summary, settings, this);
	}
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	else if (
		std::shared_ptr<SourceGroupSettingsJavaEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsJavaEmpty>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsJavaMaven> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsJavaMaven>(group))
	{
		fillSummary(summary, settings, this);
	}
	else if (
		std::shared_ptr<SourceGroupSettingsJavaGradle> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsJavaGradle>(group))
	{
		fillSummary(summary, settings, this);
	}
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
	else if (
		std::shared_ptr<SourceGroupSettingsPythonEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsPythonEmpty>(group))
	{
		fillSummary(summary, settings, this);
	}
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

	setContent(summary);

	qDeleteAll(m_contentWidget->children());
	QtProjectWizardWindow::populateWindow(m_contentWidget);

	loadContent();

	m_previouslySelectedIndex = index;
}

void QtProjectWizard::selectedSourceGroupNameChanged(QString name)
{
	m_sourceGroupList->item(m_sourceGroupList->currentRow())->setText(name);
}

void QtProjectWizard::removeSelectedSourceGroup()
{
	if (!m_allSourceGroupSettings.size() || m_sourceGroupList->currentRow() < 0)
	{
		return;
	}

	QMessageBox msgBox;
	msgBox.setText(QStringLiteral("Remove Source Group"));
	msgBox.setInformativeText(QStringLiteral("Do you really want to remove this source group from the project?"));
	msgBox.addButton(QStringLiteral("Yes"), QMessageBox::ButtonRole::YesRole);
	msgBox.addButton(QStringLiteral("No"), QMessageBox::ButtonRole::NoRole);
	msgBox.setIcon(QMessageBox::Icon::Question);
	int ret = msgBox.exec();

	if (ret == 0)	 // QMessageBox::Yes
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
			for (std::shared_ptr<SourceGroupSettings> sourceGroupSettings: m_allSourceGroupSettings)
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

void QtProjectWizard::selectedProjectType(SourceGroupType sourceGroupType)
{
	const std::string sourceGroupId = utility::getUuidString();

	switch (sourceGroupType)
	{
#if BUILD_CXX_LANGUAGE_PACKAGE
	case SOURCE_GROUP_C_EMPTY:
	{
		std::shared_ptr<SourceGroupSettingsCEmpty> settings =
			std::make_shared<SourceGroupSettingsCEmpty>(sourceGroupId, m_projectSettings.get());
		addMsvcCompatibilityFlagsOnDemand(settings);
		executeSourceGroupSetup<SourceGroupSettingsCEmpty>(settings);
	}
	break;
	case SOURCE_GROUP_CPP_EMPTY:
	{
		std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
			std::make_shared<SourceGroupSettingsCppEmpty>(sourceGroupId, m_projectSettings.get());
		addMsvcCompatibilityFlagsOnDemand(settings);
		executeSourceGroupSetup<SourceGroupSettingsCppEmpty>(settings);
	}
	break;
	case SOURCE_GROUP_CXX_CDB:
	{
		std::shared_ptr<SourceGroupSettingsCxxCdb> settings =
			std::make_shared<SourceGroupSettingsCxxCdb>(sourceGroupId, m_projectSettings.get());
		executeSourceGroupSetup<SourceGroupSettingsCxxCdb>(settings);
	}
	break;
	case SOURCE_GROUP_CXX_CODEBLOCKS:
	{
		std::shared_ptr<SourceGroupSettingsCxxCodeblocks> settings =
			std::make_shared<SourceGroupSettingsCxxCodeblocks>(
				sourceGroupId, m_projectSettings.get());
		addMsvcCompatibilityFlagsOnDemand(settings);
		executeSourceGroupSetup<SourceGroupSettingsCxxCodeblocks>(settings);
	}
	break;
	case SOURCE_GROUP_CXX_VS:
	{
		std::shared_ptr<SourceGroupSettingsCxxCdbVs> settings =
			std::make_shared<SourceGroupSettingsCxxCdbVs>(sourceGroupId, m_projectSettings.get());
		executeSourceGroupSetup<SourceGroupSettingsCxxCdbVs>(settings);
	}
	break;
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
#if BUILD_JAVA_LANGUAGE_PACKAGE
	case SOURCE_GROUP_JAVA_EMPTY:
	{
		std::shared_ptr<SourceGroupSettingsJavaEmpty> settings =
			std::make_shared<SourceGroupSettingsJavaEmpty>(sourceGroupId, m_projectSettings.get());
		executeSourceGroupSetup<SourceGroupSettingsJavaEmpty>(settings);
	}
	break;
	case SOURCE_GROUP_JAVA_MAVEN:
	{
		std::shared_ptr<SourceGroupSettingsJavaMaven> settings =
			std::make_shared<SourceGroupSettingsJavaMaven>(sourceGroupId, m_projectSettings.get());
		executeSourceGroupSetup<SourceGroupSettingsJavaMaven>(settings);
	}
	break;
	case SOURCE_GROUP_JAVA_GRADLE:
	{
		std::shared_ptr<SourceGroupSettingsJavaGradle> settings =
			std::make_shared<SourceGroupSettingsJavaGradle>(sourceGroupId, m_projectSettings.get());
		executeSourceGroupSetup<SourceGroupSettingsJavaGradle>(settings);
	}
	break;
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE
#if BUILD_PYTHON_LANGUAGE_PACKAGE
	case SOURCE_GROUP_PYTHON_EMPTY:
	{
		std::shared_ptr<SourceGroupSettingsPythonEmpty> settings =
			std::make_shared<SourceGroupSettingsPythonEmpty>(sourceGroupId, m_projectSettings.get());
		executeSourceGroupSetup<SourceGroupSettingsPythonEmpty>(settings);
	}
	break;
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE
	case SOURCE_GROUP_CUSTOM_COMMAND:
	{
		std::shared_ptr<SourceGroupSettingsCustomCommand> settings =
			std::make_shared<SourceGroupSettingsCustomCommand>(
				sourceGroupId, m_projectSettings.get());
		executeSourceGroupSetup<SourceGroupSettingsCustomCommand>(settings);
	}
	break;
	case SOURCE_GROUP_UNKNOWN:
		break;
	}
}

void QtProjectWizard::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings)
{
	m_allSourceGroupSettings.push_back(settings);

	updateSourceGroupList();
	cancelSourceGroup();

	setNextEnabled(true);

	m_previouslySelectedIndex = -1;

	m_sourceGroupList->setCurrentRow(m_allSourceGroupSettings.size() - 1);
}

void QtProjectWizard::createProject()
{
	FilePath path = m_projectSettings->getFilePath();

	m_projectSettings->setVersion(ProjectSettings::VERSION);
	m_projectSettings->setAllSourceGroupSettings(m_allSourceGroupSettings);
	m_projectSettings->save(path);

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
