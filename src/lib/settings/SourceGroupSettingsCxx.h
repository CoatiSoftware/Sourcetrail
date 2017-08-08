#ifndef SOURCE_GROUP_SETTINGS_CXX_H
#define SOURCE_GROUP_SETTINGS_CXX_H

#include "settings/SourceGroupSettings.h"

class SourceGroupSettingsCxx
	: public SourceGroupSettings
{
public:
	static std::vector<std::string> getAvailableArchTypes();
	static std::vector<std::string> getAvailableVendorTypes();
	static std::vector<std::string> getAvailableOsTypes();
	static std::vector<std::string> getAvailableEnvironmentTypes();

	SourceGroupSettingsCxx(const std::string& id, SourceGroupType type, const ProjectSettings* projectSettings);
	virtual ~SourceGroupSettingsCxx();

	virtual bool equals(std::shared_ptr<SourceGroupSettings> other) const;

	virtual std::vector<std::string> getAvailableLanguageStandards() const;

	std::vector<FilePath> getHeaderSearchPaths() const;
	std::vector<FilePath> getHeaderSearchPathsExpandedAndAbsolute() const;
	void setHeaderSearchPaths(const std::vector<FilePath>& headerSearchPaths);

	std::vector<FilePath> getFrameworkSearchPaths() const;
	std::vector<FilePath> getFrameworkSearchPathsExpandedAndAbsolute() const;
	void setFrameworkSearchPaths(const std::vector<FilePath>& frameworkSearchPaths);

	bool getTargetOptionsEnabled() const;
	void setTargetOptionsEnabled(bool targetOptionsEnabled);

	std::string getTargetArch() const;
	void setTargetArch(const std::string& arch);

	std::string getTargetVendor() const;
	void setTargetVendor(const std::string& vendor);

	std::string getTargetSys() const;
	void setTargetSys(const std::string& sys);

	std::string getTargetAbi() const;
	void setTargetAbi(const std::string& abi);

	std::string getTargetFlag() const;

	std::vector<std::string> getCompilerFlags() const;
	void setCompilerFlags(const std::vector<std::string>& compilerFlags);

	// deprecated begin
	bool getUseSourcePathsForHeaderSearch() const;
	void setUseSourcePathsForHeaderSearch(bool useSourcePathsForHeaderSearch);
	bool getHasDefinedUseSourcePathsForHeaderSearch() const;
	void setHasDefinedUseSourcePathsForHeaderSearch(bool hasDefinedUseSourcePathsForHeaderSearch);
	// deprecated end

	FilePath getCompilationDatabasePath() const;
	FilePath getCompilationDatabasePathExpandedAndAbsolute() const;
	void setCompilationDatabasePath(const FilePath& compilationDatabasePath);

	bool getShouldApplyAnonymousTypedefTransformation() const;
	void setShouldApplyAnonymousTypedefTransformation(bool shouldApplyAnonymousTypedefTransformation);

private:
	virtual std::vector<std::string> getDefaultSourceExtensions() const;
	virtual std::string getDefaultStandard() const;

	std::vector<FilePath> m_headerSearchPaths;
	std::vector<FilePath> m_frameworkSearchPaths;
	bool m_targetOptionsEnabled;
	std::string m_targetArch;
	std::string m_targetSub;
	std::string m_targetVendor;
	std::string m_targetSys;
	std::string m_targetAbi;
	std::vector<std::string> m_compilerFlags;
	bool m_useSourcePathsForHeaderSearch;
	bool m_hasDefinedUseSourcePathsForHeaderSearch;
	FilePath m_compilationDatabasePath;
	bool m_shouldApplyAnonymousTypedefTransformation;
};

#endif // SOURCE_GROUP_SETTINGS_CXX_H
