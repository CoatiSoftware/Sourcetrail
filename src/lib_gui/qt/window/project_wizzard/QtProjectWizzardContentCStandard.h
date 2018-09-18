#ifndef QT_PROJECT_WIZZARD_CONTENT_C_STANDARD
#define QT_PROJECT_WIZZARD_CONTENT_C_STANDARD

#include "QtProjectWizzardContent.h"

class QComboBox;
class SourceGroupSettingsWithCStandard;

class QtProjectWizzardContentCStandard
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentCStandard(
		std::shared_ptr<SourceGroupSettingsWithCStandard> sourceGroupSettings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettingsWithCStandard> m_sourceGroupSettings;

	QComboBox* m_standard;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_C_STANDARD
