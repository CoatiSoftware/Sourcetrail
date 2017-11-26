#include "settings/ApplicationSettings.h"

#include "License.h"
#include "settings/migration/SettingsMigrator.h"
#include "settings/migration/SettingsMigrationLambda.h"
#include "settings/migration/SettingsMigrationMoveKey.h"
#include "utility/AppPath.h"
#include "utility/ResourcePaths.h"
#include "utility/Status.h"
#include "utility/TimeStamp.h"
#include "utility/utility.h"
#include "utility/UserPaths.h"
#include "utility/Version.h"

const size_t ApplicationSettings::VERSION = 3;

std::shared_ptr<ApplicationSettings> ApplicationSettings::s_instance;

std::shared_ptr<ApplicationSettings> ApplicationSettings::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ApplicationSettings>(new ApplicationSettings());
	}

	return s_instance;
}

ApplicationSettings::ApplicationSettings()
{
}

ApplicationSettings::~ApplicationSettings()
{
}

bool ApplicationSettings::load(const FilePath& filePath)
{
	bool loaded = Settings::load(filePath);
	if (!loaded)
	{
		return false;
	}

	SettingsMigrator migrator;

	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>(
		"source/header_search_paths/header_search_path",
		"indexing/cxx/header_search_paths/header_search_path"
	));
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>(
		"source/framework_search_paths/framework_search_path",
		"indexing/cxx/framework_search_paths/framework_search_path"
	));
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>(
		"application/indexer_thread_count",
		"indexing/indexer_thread_count"
	));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>(
		"network/coati_port",
		"network/sourcetrail_port"
	));
	migrator.addMigration(3, std::make_shared<SettingsMigrationLambda>(
		[](const SettingsMigration* migration, Settings* settings)
		{
			License license;
			bool isLoaded = license.loadFromEncodedString(
				migration->getValueFromSettings<std::string>(settings, "user/license/license", ""), AppPath::getAppPath());
			if (isLoaded && license.isValid() && license.isNonCommercialLicenseType())
			{
				migration->removeValuesInSettings(settings, "user/license/license");
				migration->removeValuesInSettings(settings, "user/license/check");
				migration->setValueInSettings(settings, "user/license/non_commercial_use", true);
			}
		}
	));

	bool migrated = migrator.migrate(this, ApplicationSettings::VERSION);

	if (migrated)
	{
		save();
	}

	return true;
}

bool ApplicationSettings::operator==(const ApplicationSettings& other) const
{
	return
		utility::isPermutation<FilePath>(getHeaderSearchPaths(), other.getHeaderSearchPaths()) &&
		utility::isPermutation<FilePath>(getFrameworkSearchPaths(), other.getFrameworkSearchPaths());
}

int ApplicationSettings::getMaxRecentProjectsCount() const
{
	return 7;
}

std::string ApplicationSettings::getFontName() const
{
	return getValue<std::string>("application/font_name", "Source Code Pro");
}

void ApplicationSettings::setFontName(const std::string& fontName)
{
	setValue<std::string>("application/font_name", fontName);
}

int ApplicationSettings::getFontSize() const
{
	return getValue<int>("application/font_size", 14);
}

void ApplicationSettings::setFontSize(int fontSize)
{
	setValue<int>("application/font_size", fontSize);
}

std::string ApplicationSettings::getTextEncoding() const
{
	return getValue<std::string>("application/text_encoding", "UTF-8");
}

void ApplicationSettings::setTextEncoding(const std::string& textEncoding)
{
	setValue<std::string>("application/text_encoding", textEncoding);
}

bool ApplicationSettings::getUseAnimations() const
{
	return getValue<bool>("application/use_animations", true);
}

void ApplicationSettings::setUseAnimations(bool useAnimations)
{
	setValue<bool>("application/use_animations", useAnimations);
}

bool ApplicationSettings::getShowBuiltinTypesInGraph() const
{
	return getValue<bool>("application/builtin_types_in_graph", false);
}

void ApplicationSettings::setShowBuiltinTypesInGraph(bool showBuiltinTypes)
{
	setValue<bool>("application/builtin_types_in_graph", showBuiltinTypes);
}

FilePath ApplicationSettings::getColorSchemePath() const
{
	FilePath defaultPath(ResourcePaths::getColorSchemesPath().concat(FilePath("bright.xml")));
	FilePath path(getValue<std::string>("application/color_scheme", defaultPath.str()));

	if (path != defaultPath && !path.exists())
	{
		return defaultPath;
	}

	return path;
}

void ApplicationSettings::setColorSchemePath(const FilePath& colorSchemePath)
{
	setValue<std::string>("application/color_scheme", colorSchemePath.str());
}

int ApplicationSettings::getFontSizeMax() const
{
	return getValue<int>("application/font_size_max", 24);
}

void ApplicationSettings::setFontSizeMax(const int fontSizeMax)
{
	setValue<int>("application/font_size_max", fontSizeMax);
}

int ApplicationSettings::getFontSizeMin() const
{
	return getValue<int>("application/font_size_min", 4);
}

void ApplicationSettings::setFontSizeMin(const int fontSizeMin)
{
	setValue<int>("application/font_size_min", fontSizeMin);
}

int ApplicationSettings::getFontSizeStd() const
{
	return getValue<int>("application/font_size_std", 12);
}

void ApplicationSettings::setFontSizeStd(const int fontSizeStd)
{
	setValue<int>("application/font_size_std", fontSizeStd);
}

int ApplicationSettings::getWindowBaseWidth() const
{
	return getValue<int>("application/window_base_width", 500);
}

int ApplicationSettings::getWindowBaseHeight() const
{
	return getValue<int>("application/window_base_height", 500);
}

float ApplicationSettings::getScrollSpeed() const
{
	return getValue<float>("application/scroll_speed", 1.0f);
}

void ApplicationSettings::setScrollSpeed(float scrollSpeed)
{
	setValue<float>("application/scroll_speed", scrollSpeed);
}

bool ApplicationSettings::getGraphControlsVisible() const
{
	return getValue<bool>("application/graph_controls_visible", true);
}

void ApplicationSettings::setGraphControlsVisible(bool visible)
{
	setValue<bool>("application/graph_controls_visible", visible);
}

bool ApplicationSettings::getLoggingEnabled() const
{
	return getValue<bool>("application/logging_enabled", false);
}

void ApplicationSettings::setLoggingEnabled(bool value)
{
	setValue<bool>("application/logging_enabled", value);
}

bool ApplicationSettings::getVerboseIndexerLoggingEnabled() const
{
	return getValue<bool>("application/verbose_indexer_logging_enabled", false);
}

void ApplicationSettings::setVerboseIndexerLoggingEnabled(bool value)
{
	setValue<bool>("application/verbose_indexer_logging_enabled", value);
}

void ApplicationSettings::setLogFilter(int mask)
{
	setValue<int>("application/log_filter", mask);
}

void ApplicationSettings::setStatusFilter(int mask)
{
	setValue<int>("application/status_filter", mask);
}

int ApplicationSettings::getStatusFilter() const
{
	return getValue<int>("application/status_filter", StatusType::STATUS_INFO | StatusType::STATUS_ERROR);
}

int ApplicationSettings::getLogFilter() const
{
	return getValue<int>("application/log_filter", Logger::LOG_WARNINGS | Logger::LOG_ERRORS);
}

std::vector<FilePath> ApplicationSettings::getIndexingFilePaths() const
{
	return getPathValues("application/state/indexing_paths/indexing_path");
}

bool ApplicationSettings::setIndexingFilePaths(const std::vector<FilePath>& indexingFiles)
{
	return setPathValues("application/state/indexing_paths/indexing_path", indexingFiles);
}

int ApplicationSettings::getIndexerThreadCount() const
{
	return getValue<int>("indexing/indexer_thread_count", 0);
}

void ApplicationSettings::setIndexerThreadCount(const int count)
{
	setValue<int>("indexing/indexer_thread_count", count);
}

bool ApplicationSettings::getMultiProcessIndexingEnabled() const
{
	return getValue<bool>("indexing/multi_process_indexing", true);
}

void ApplicationSettings::setMultiProcessIndexingEnabled(bool enabled)
{
	setValue<bool>("indexing/multi_process_indexing", enabled);
}

std::string ApplicationSettings::getJavaPath() const
{
	return getValue<std::string>("indexing/java/java_path", "");
}

void ApplicationSettings::setJavaPath(const FilePath& path)
{
	setValue<std::string>("indexing/java/java_path", path.str());
}

int ApplicationSettings::getJavaMaximumMemory() const
{
	return getValue<int>("indexing/java/java_maximum_memory", -1);
}

void ApplicationSettings::setJavaMaximumMemory(int size)
{
	setValue<int>("indexing/java/java_maximum_memory", size);
}

std::vector<FilePath> ApplicationSettings::getJreSystemLibraryPaths() const
{
	return getPathValues("indexing/java/jre_system_library_paths/jre_system_library_path");
}

std::vector<FilePath> ApplicationSettings::getJreSystemLibraryPathsExpanded() const
{
	return expandPaths(getJreSystemLibraryPaths());
}

bool ApplicationSettings::setJreSystemLibraryPaths(const std::vector<FilePath>& jreSystemLibraryPaths)
{
	return setPathValues("indexing/java/jre_system_library_paths/jre_system_library_path", jreSystemLibraryPaths);
}

FilePath ApplicationSettings::getMavenPath() const
{
	return FilePath(getValue<std::string>("indexing/java/maven_path", ""));
}

void ApplicationSettings::setMavenPath(const FilePath& path)
{
	setValue<std::string>("indexing/java/maven_path", path.str());
}

std::vector<FilePath> ApplicationSettings::getHeaderSearchPaths() const
{
	return getPathValues("indexing/cxx/header_search_paths/header_search_path");
}

std::vector<FilePath> ApplicationSettings::getHeaderSearchPathsExpanded() const
{
	return expandPaths(getHeaderSearchPaths());
}

bool ApplicationSettings::setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths)
{
	return setPathValues("indexing/cxx/header_search_paths/header_search_path", headerSearchPaths);
}

std::vector<FilePath> ApplicationSettings::getFrameworkSearchPaths() const
{
	return getPathValues("indexing/cxx/framework_search_paths/framework_search_path");
}

std::vector<FilePath> ApplicationSettings::getFrameworkSearchPathsExpanded() const
{
	return expandPaths(getFrameworkSearchPaths());
}

bool ApplicationSettings::setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths)
{
	return setPathValues("indexing/cxx/framework_search_paths/framework_search_path", frameworkSearchPaths);
}

int ApplicationSettings::getCodeTabWidth() const
{
	return getValue<int>("code/tab_width", 4);
}

void ApplicationSettings::setCodeTabWidth(int codeTabWidth)
{
	setValue<int>("code/tab_width", codeTabWidth);
}

int ApplicationSettings::getCodeSnippetSnapRange() const
{
	return getValue<int>("code/snippet/snap_range", 4);
}

void ApplicationSettings::setCodeSnippetSnapRange(int range)
{
	setValue<int>("code/snippet/snap_range", range);
}

int ApplicationSettings::getCodeSnippetExpandRange() const
{
	return getValue<int>("code/snippet/expand_range", 3);
}

void ApplicationSettings::setCodeSnippetExpandRange(int range)
{
	setValue<int>("code/snippet/expand_range", range);
}

bool ApplicationSettings::getCodeViewModeSingle() const
{
	return getValue<bool>("code/view_mode_single", false);
}

void ApplicationSettings::setCodeViewModeSingle(bool enabled)
{
	setValue<bool>("code/view_mode_single", enabled);
}

std::vector<FilePath> ApplicationSettings::getRecentProjects() const
{
	std::vector<FilePath> recentProjects;
	std::vector<FilePath> loadedRecentProjects = getPathValues("user/recent_projects/recent_project");

	for (const FilePath& project: loadedRecentProjects)
	{
		if (project.isAbsolute())
		{
			recentProjects.push_back(project);
		}
		else
		{
			recentProjects.push_back(UserPaths::getUserDataPath().concat(project));
		}
	}
	return recentProjects;
}

bool ApplicationSettings::setRecentProjects(const std::vector<FilePath> &recentProjects)
{
	return setPathValues("user/recent_projects/recent_project", recentProjects);
}

int ApplicationSettings::getAcceptedEulaVersion() const
{
	return getValue<int>("user/accepted_eula_version", 0);
}

void ApplicationSettings::setAcceptedEulaVersion(int version)
{
	setValue<int>("user/accepted_eula_version", version);
}

std::string ApplicationSettings::getUserToken() const
{
	return getValue<std::string>("user/token", "");
}

void ApplicationSettings::setUserToken(std::string token)
{
	setValue<std::string>("user/token", token);
}

bool ApplicationSettings::getAutomaticUpdateCheck() const
{
	return getValue<bool>("user/update_check/automatic", true);
}

void ApplicationSettings::setAutomaticUpdateCheck(bool automaticUpdates)
{
	setValue<bool>("user/update_check/automatic", automaticUpdates);
}

TimeStamp ApplicationSettings::getLastUpdateCheck() const
{
	std::string val = getValue<std::string>("user/update_check/time_stamp", "");
	if (!val.size())
	{
		val = getValue<std::string>("user/update_check/last", ""); // deprecated key
	}

	return TimeStamp(val);
}

void ApplicationSettings::setLastUpdateCheck(const TimeStamp& time)
{
	setValue<std::string>("user/update_check/time_stamp", time.toString());
}

Version ApplicationSettings::getSkipUpdateForVersion() const
{
	return Version::fromString(getValue<std::string>("user/update_check/skip_version", "2017.1.0"));
}

void ApplicationSettings::setSkipUpdateForVersion(const Version& version)
{
	if (version.isValid())
	{
		setValue<std::string>("user/update_check/skip_version", version.toDisplayString());
	}
}

std::string ApplicationSettings::getUpdateDownloadUrl() const
{
	return getValue<std::string>("user/update_check/update_url", "");
}

void ApplicationSettings::setUpdateDownloadUrl(const std::string& url)
{
	setValue<std::string>("user/update_check/update_url", url);
}

Version ApplicationSettings::getUpdateVersion() const
{
	return Version::fromString(getValue<std::string>("user/update_check/update_version", "2017.1.0"));
}

void ApplicationSettings::setUpdateVersion(const Version& version)
{
	if (version.isValid())
	{
		setValue<std::string>("user/update_check/update_version", version.toDisplayString());
	}
}

bool ApplicationSettings::getSeenErrorHelpMessage() const
{
	return getValue<bool>("user/seen_error_help_message", false);
}

void ApplicationSettings::setSeenErrorHelpMessage(bool seen)
{
	setValue<bool>("user/seen_error_help_message", seen);
}

int ApplicationSettings::getPluginPort() const
{
	return getValue<int>("network/plugin_port", 6666);
}

void ApplicationSettings::setPluginPort(const int pluginPort)
{
	setValue<int>("network/plugin_port", pluginPort);
}

int ApplicationSettings::getSourcetrailPort() const
{
	return getValue<int>("network/sourcetrail_port", 6667);
}

void ApplicationSettings::setSourcetrailPort(const int sourcetrailPort)
{
	setValue<int>("network/sourcetrail_port", sourcetrailPort);
}

int ApplicationSettings::getControlsMouseBackButton() const
{
	return getValue<int>("controls/mouse_back_button", 0x8);
}

int ApplicationSettings::getControlsMouseForwardButton() const
{
	return getValue<int>("controls/mouse_forward_button", 0x10);
}

bool ApplicationSettings::getControlsGraphZoomOnMouseWheel() const
{
	return getValue<bool>("controls/graph_zoom_on_mouse_wheel", false);
}

void ApplicationSettings::setControlsGraphZoomOnMouseWheel(bool zoomingDefault)
{
	setValue<bool>("controls/graph_zoom_on_mouse_wheel", zoomingDefault);
}

std::string ApplicationSettings::getLicenseString() const
{
	return getValue<std::string>("user/license/license", "");
}

void ApplicationSettings::setLicenseString(const std::string& licenseString)
{
	setValue<std::string>("user/license/license", licenseString);
}

std::string ApplicationSettings::getLicenseCheck() const
{
	return getValue<std::string>("user/license/check", "");
}

void ApplicationSettings::setLicenseCheck(const std::string& hash)
{
	setValue<std::string>("user/license/check", hash);
}

bool ApplicationSettings::getNonCommercialUse() const
{
	return getValue<bool>("user/license/non_commercial_use", false);
}

void ApplicationSettings::setNonCommercialUse(bool nonCommercialUse)
{
	setValue<bool>("user/license/non_commercial_use", nonCommercialUse);
}
