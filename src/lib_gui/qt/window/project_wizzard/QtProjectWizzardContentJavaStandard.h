#ifndef QT_PROJECT_WIZZARD_CONTENT_JAVA_STANDARD
#define QT_PROJECT_WIZZARD_CONTENT_JAVA_STANDARD

#include "QtProjectWizzardContent.h"

class QComboBox;
class SourceGroupSettingsWithJavaStandard;

class QtProjectWizzardContentJavaStandard
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentJavaStandard(
		std::shared_ptr<SourceGroupSettingsWithJavaStandard> sourceGroupSettings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettingsWithJavaStandard> m_sourceGroupSettings;

	QComboBox* m_standard;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_JAVA_STANDARD
