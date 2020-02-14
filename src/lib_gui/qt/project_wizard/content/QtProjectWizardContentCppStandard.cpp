#include "QtProjectWizardContentCppStandard.h"

#include <QComboBox>
#include <QLabel>

#include "SourceGroupSettingsWithCppStandard.h"

QtProjectWizardContentCppStandard::QtProjectWizardContentCppStandard(
	std::shared_ptr<SourceGroupSettingsWithCppStandard> sourceGroupSettings,
	QtProjectWizardWindow* window)
	: QtProjectWizardContent(window), m_sourceGroupSettings(sourceGroupSettings), m_standard(nullptr)
{
}

void QtProjectWizardContentCppStandard::populate(QGridLayout* layout, int& row)
{
	m_standard = new QComboBox();
	layout->addWidget(
		createFormLabel(QStringLiteral("C++ Standard")),
		row,
		QtProjectWizardWindow::FRONT_COL,
		Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft);
	row++;
}

void QtProjectWizardContentCppStandard::load()
{
	m_standard->clear();

	if (m_sourceGroupSettings)
	{
		std::vector<std::wstring> standards = m_sourceGroupSettings->getAvailableCppStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(static_cast<int>(i), QString::fromStdWString(standards[i]));
		}

		m_standard->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getCppStandard()));
	}
}

void QtProjectWizardContentCppStandard::save()
{
	if (m_sourceGroupSettings)
	{
		m_sourceGroupSettings->setCppStandard(m_standard->currentText().toStdWString());
	}
}
