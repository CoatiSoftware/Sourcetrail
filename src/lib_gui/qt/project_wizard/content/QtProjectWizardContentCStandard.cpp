#include "QtProjectWizardContentCStandard.h"

#include <QComboBox>
#include <QLabel>

#include "SourceGroupSettingsWithCStandard.h"

QtProjectWizardContentCStandard::QtProjectWizardContentCStandard(
	std::shared_ptr<SourceGroupSettingsWithCStandard> sourceGroupSettings,
	QtProjectWizardWindow* window)
	: QtProjectWizardContent(window), m_sourceGroupSettings(sourceGroupSettings), m_standard(nullptr)
{
}

void QtProjectWizardContentCStandard::populate(QGridLayout* layout, int& row)
{
	m_standard = new QComboBox();
	layout->addWidget(
		createFormLabel(QStringLiteral("C Standard")),
		row,
		QtProjectWizardWindow::FRONT_COL,
		Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft);
	row++;
}

void QtProjectWizardContentCStandard::load()
{
	m_standard->clear();

	if (m_sourceGroupSettings)
	{
		std::vector<std::wstring> standards = m_sourceGroupSettings->getAvailableCStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(static_cast<int>(i), QString::fromStdWString(standards[i]));
		}

		m_standard->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getCStandard()));
	}
}

void QtProjectWizardContentCStandard::save()
{
	if (m_sourceGroupSettings)
	{
		m_sourceGroupSettings->setCStandard(m_standard->currentText().toStdWString());
	}
}
