#ifndef QT_PROJECT_WIZARD_CONTENT_CROSS_COMPILATION_OPTIONS
#define QT_PROJECT_WIZARD_CONTENT_CROSS_COMPILATION_OPTIONS

#include "QtProjectWizardContent.h"

class QCheckBox;
class QComboBox;
class QLabel;
class SourceGroupSettingsWithCxxCrossCompilationOptions;

class QtProjectWizardContentCrossCompilationOptions: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentCrossCompilationOptions(
		std::shared_ptr<SourceGroupSettingsWithCxxCrossCompilationOptions> sourceGroupSettings,
		QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	void onUseTargetOptionsChanged();
	void updateTargetOptionsEnabled();

	std::shared_ptr<SourceGroupSettingsWithCxxCrossCompilationOptions> m_sourceGroupSettings;

	QCheckBox* m_useTargetOptions;
	QComboBox* m_arch;
	QComboBox* m_vendor;
	QComboBox* m_sys;
	QComboBox* m_abi;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_CROSS_COMPILATION_OPTIONS
