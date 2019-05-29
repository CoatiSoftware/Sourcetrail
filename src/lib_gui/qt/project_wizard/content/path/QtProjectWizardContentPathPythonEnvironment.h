#ifndef QT_PROJECT_WIZARD_CONTENT_PATH_PYTHON_ENVIRONMENT_H
#define QT_PROJECT_WIZARD_CONTENT_PATH_PYTHON_ENVIRONMENT_H

#include "QtProjectWizardContentPath.h"
#include "QtThreadedFunctor.h"

class SourceGroupSettingsPythonEmpty;

class QtProjectWizardContentPathPythonEnvironment
	: public QtProjectWizardContentPath
{
	Q_OBJECT
public:
	QtProjectWizardContentPathPythonEnvironment(std::shared_ptr<SourceGroupSettingsPythonEmpty> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override;
	void load() override;
	void save() override;

private:
	void onTextChanged(const QString& text);
	std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override;

	std::shared_ptr<SourceGroupSettingsPythonEmpty> m_settings;
	QtThreadedLambdaFunctor m_onQtThread;
	QLabel* m_resultLabel;
};

#endif // QT_PROJECT_WIZARD_CONTENT_PATH_PYTHON_ENVIRONMENT_H
