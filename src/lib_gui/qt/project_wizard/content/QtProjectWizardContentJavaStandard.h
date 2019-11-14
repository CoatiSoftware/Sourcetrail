#ifndef QT_PROJECT_WIZARD_CONTENT_JAVA_STANDARD
#define QT_PROJECT_WIZARD_CONTENT_JAVA_STANDARD

#include "QtProjectWizardContent.h"

class QComboBox;
class SourceGroupSettingsWithJavaStandard;

class QtProjectWizardContentJavaStandard
	: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentJavaStandard(
		std::shared_ptr<SourceGroupSettingsWithJavaStandard> sourceGroupSettings,
		QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;

private:
	std::shared_ptr<SourceGroupSettingsWithJavaStandard> m_sourceGroupSettings;

	QComboBox* m_standard;
};

#endif // QT_PROJECT_WIZARD_CONTENT_JAVA_STANDARD
