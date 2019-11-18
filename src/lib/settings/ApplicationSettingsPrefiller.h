#ifndef APPLICATION_SETTINGS_PREFILLER_H
#define APPLICATION_SETTINGS_PREFILLER_H

class ApplicationSettings;

class ApplicationSettingsPrefiller
{
public:
	static void prefillPaths(ApplicationSettings* settings);

private:
	static bool prefillJavaRuntimePath(ApplicationSettings* settings);
	static bool prefillJreSystemLibraryPaths(ApplicationSettings* settings);
	static bool prefillMavenExecutablePath(ApplicationSettings* settings);
	static bool prefillCxxHeaderPaths(ApplicationSettings* settings);
	static bool prefillCxxFrameworkPaths(ApplicationSettings* settings);
};

#endif	  // APPLICATION_SETTINGS_PREFILLER_H
