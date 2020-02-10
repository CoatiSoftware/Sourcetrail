#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_H

#include <set>

#include "QtLocationPicker.h"
#include "QtProjectWizardContent.h"

class SourceGroupSettings;

class QtProjectWizardContentPath: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentPath(QtProjectWizardWindow* window);

	// QtSettingsWindow implementation
	virtual void populate(QGridLayout* layout, int& row) override;

	virtual bool check() override;

protected:
	void setTitleString(const QString& title);
	void setHelpString(const QString& help);
	void setPlaceholderString(const QString& placeholder);

	void setFileEndings(const std::set<std::wstring>& fileEndings);

	QtLocationPicker* m_picker;

private:
	virtual std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() = 0;

	QString m_titleString;
	QString m_helpString;
	QString m_placeholderString;
	std::set<std::wstring> m_fileEndings;
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_PATH_H
