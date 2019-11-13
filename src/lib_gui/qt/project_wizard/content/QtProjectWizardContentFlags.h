#ifndef QT_PROJECT_WIZARD_CONTENT_FLAGS_H
#define QT_PROJECT_WIZARD_CONTENT_FLAGS_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "QtProjectWizardContent.h"

class QtStringListBox;
class SourceGroupSettingsWithCxxPathsAndFlags;

class QtProjectWizardContentFlags
	: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentFlags(
		std::shared_ptr<SourceGroupSettingsWithCxxPathsAndFlags> settings,
		QtProjectWizardWindow* window,
		bool indicateAsAdditional = false
	);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual void load() override;
	virtual void save() override;
	virtual bool check() override;

private:
	std::shared_ptr<SourceGroupSettingsWithCxxPathsAndFlags> m_settings;
	const bool m_indicateAsAdditional;

	QtStringListBox* m_list;
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_FLAGS_H
