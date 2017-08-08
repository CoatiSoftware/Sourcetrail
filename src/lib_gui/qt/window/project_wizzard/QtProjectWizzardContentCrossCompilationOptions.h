#ifndef QT_PROJECT_WIZZARD_CONTENT_CROSS_COMPILATION_OPTIONS
#define QT_PROJECT_WIZZARD_CONTENT_CROSS_COMPILATION_OPTIONS

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QCheckBox;
class QComboBox;
class QLabel;
class SourceGroupSettings;

class QtProjectWizzardContentCrossCompilationOptions
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentCrossCompilationOptions(
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	void onUseTargetOptionsChanged();
	void updateTargetOptionsEnabled();

	std::shared_ptr<SourceGroupSettings> m_sourceGroupSettings;

	QCheckBox* m_useTargetOptions;
	QComboBox* m_arch;
	QComboBox* m_vendor;
	QComboBox* m_sys;
	QComboBox* m_abi;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_CROSS_COMPILATION_OPTIONS
