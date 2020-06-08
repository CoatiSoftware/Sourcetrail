#include "QtProjectWizardContentExtensions.h"

#include "language_packages.h"

#include <QFormLayout>

#include "QtStringListBox.h"

#if BUILD_CXX_LANGUAGE_PACKAGE
#	include "SourceGroupSettingsWithSourceExtensions.h"
#	include "SourceGroupSettingsWithSourceExtensionsC.h"
#	include "SourceGroupSettingsWithSourceExtensionsCpp.h"
#	include "SourceGroupSettingsWithSourceExtensionsCxx.h"
#endif

QtProjectWizardContentExtensions::QtProjectWizardContentExtensions(
	std::shared_ptr<SourceGroupSettingsWithSourceExtensions> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContent(window), m_settings(settings)
{
}

void QtProjectWizardContentExtensions::populate(QGridLayout* layout, int& row)
{
	QLabel* sourceLabel = createFormLabel(QStringLiteral("Source File Extensions"));
	layout->addWidget(sourceLabel, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);

	QString cxxAddition("");
	if (std::dynamic_pointer_cast<SourceGroupSettingsWithSourceExtensionsC>(m_settings) ||
		std::dynamic_pointer_cast<SourceGroupSettingsWithSourceExtensionsCpp>(m_settings) ||
		std::dynamic_pointer_cast<SourceGroupSettingsWithSourceExtensionsCxx>(m_settings))
	{
		cxxAddition = QStringLiteral(
			" Files with these extensions will serve as entry points for the indexer. Headers that "
			"are included by these files will be traversed on the fly.");
	}

	addHelpButton(
		QStringLiteral("Source File Extensions"),
		QStringLiteral(
			"Define extensions for source files including the dot (e.g. \".cpp\" or \".java\").") +
			cxxAddition,
		layout,
		row);

	m_listBox = new QtStringListBox(this, sourceLabel->text());
	layout->addWidget(m_listBox, row, QtProjectWizardWindow::BACK_COL);
	row++;
}

void QtProjectWizardContentExtensions::load()
{
	m_listBox->setStrings(m_settings->getSourceExtensions());
}

void QtProjectWizardContentExtensions::save()
{
	m_settings->setSourceExtensions(m_listBox->getStrings());
}
