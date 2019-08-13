#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_CXX_PCH_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_CXX_PCH_H

#include "QtProjectWizardContentPath.h"
#include "SingleValueCache.h"

class SourceGroupSettingsWithCxxPchOptions;

class QtProjectWizardContentPathCxxPch
	: public QtProjectWizardContentPath
{
	Q_OBJECT

public:
	QtProjectWizardContentPathCxxPch(
		std::shared_ptr<SourceGroupSettings> settings,
		std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> settingsCxxPch,
		QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;

	void load() override;
	void save() override;
	bool check() override;

private:
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettings> m_settings;
	std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> m_settingsCxxPch;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_CXX_PCH_H
