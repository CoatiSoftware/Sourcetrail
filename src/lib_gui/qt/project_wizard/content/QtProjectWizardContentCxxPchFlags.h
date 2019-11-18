#ifndef QT_PROJECT_WIZARD_CONTENT_CXX_PCH_FLAGS_H
#define QT_PROJECT_WIZARD_CONTENT_CXX_PCH_FLAGS_H

#include "QtProjectWizardContent.h"

class QCheckBox;
class QtStringListBox;
class SourceGroupSettingsWithCxxPchOptions;

class QtProjectWizardContentCxxPchFlags: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentCxxPchFlags(
		std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> settings,
		QtProjectWizardWindow* window,
		bool isCDB);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> m_settings;
	const bool m_isCDB;

	QCheckBox* m_useCompilerFlags;
	QtStringListBox* m_list;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_CXX_PCH_FLAGS_H
