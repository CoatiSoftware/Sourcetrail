#ifndef QT_PROJECT_WIZZARD_CONTENT_CPP_STANDARD
#define QT_PROJECT_WIZZARD_CONTENT_CPP_STANDARD

#include "QtProjectWizzardContent.h"

class QComboBox;
class SourceGroupSettingsWithCppStandard;

class QtProjectWizzardContentCppStandard
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentCppStandard(
		std::shared_ptr<SourceGroupSettingsWithCppStandard> sourceGroupSettings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettingsWithCppStandard> m_sourceGroupSettings;

	QComboBox* m_standard;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_CPP_STANDARD
