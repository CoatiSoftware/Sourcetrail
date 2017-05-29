#ifndef QT_PROJECT_WIZZARD_CONTENT_LANGUAGE_AND_STANDARD
#define QT_PROJECT_WIZZARD_CONTENT_LANGUAGE_AND_STANDARD

#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

class QComboBox;
class QLabel;
class SourceGroupSettings;

class QtProjectWizzardContentLanguageAndStandard
	: public QtProjectWizzardContent
{
	Q_OBJECT

public:
	QtProjectWizzardContentLanguageAndStandard(
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings,
		QtProjectWizzardWindow* window);

	// QtProjectWizzardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	std::shared_ptr<SourceGroupSettings> m_sourceGroupSettings;

	QLabel* m_language;
	QComboBox* m_standard;
};

#endif // QT_PROJECT_WIZZARD_CONTENT_LANGUAGE_AND_STANDARD
