#include "QtProjectWizardContentExtensions.h"

#include <QFormLayout>

#include "QtStringListBox.h"
#include "SourceGroupSettingsWithSourceExtensions.h"

QtProjectWizardContentExtensions::QtProjectWizardContentExtensions(
	std::shared_ptr<SourceGroupSettingsWithSourceExtensions> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContent(window), m_settings(settings)
{
}

void QtProjectWizardContentExtensions::populate(QGridLayout* layout, int& row)
{
	QLabel* sourceLabel = createFormLabel(QStringLiteral("Source File Extensions"));
	layout->addWidget(sourceLabel, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);

	addHelpButton(
		QStringLiteral("Source File Extensions"),
		QStringLiteral("Define extensions for source files including the dot (e.g. .cpp or .java)"),
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
