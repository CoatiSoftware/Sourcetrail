#ifndef QT_PROJECT_WIZARD_CONTENT_PATHS_CLASS_JAVA_H
#define QT_PROJECT_WIZARD_CONTENT_PATHS_CLASS_JAVA_H

#include "language_packages.h"

#if BUILD_JAVA_LANGUAGE_PACKAGE

#include "QtProjectWizardContentPaths.h"

class QCheckBox;

class QtProjectWizardContentPathsClassJava
	: public QtProjectWizardContentPaths
{
	Q_OBJECT
public:
	QtProjectWizardContentPathsClassJava(std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;
	virtual void load() override;
	virtual void save() override;

private:
	QCheckBox* m_useJreSystemLibraryCheckBox;
};

#endif // BUILD_JAVA_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_PATHS_CLASS_JAVA_H
