#include "qt/window/project_wizzard/QtProjectWizzardContentCStandard.h"

#include <QComboBox>
#include <QLabel>

#include "settings/SourceGroupSettingsWithCStandard.h"

QtProjectWizzardContentCStandard::QtProjectWizzardContentCStandard(
	std::shared_ptr<SourceGroupSettingsWithCStandard> sourceGroupSettings,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_sourceGroupSettings(sourceGroupSettings)
	, m_standard(nullptr)
{
}

void QtProjectWizzardContentCStandard::populate(QGridLayout* layout, int& row)
{
	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	m_standard = new QComboBox();
	layout->addWidget(createFormLabel("C Standard"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizzardContentCStandard::load()
{
	m_standard->clear();

	if (m_sourceGroupSettings)
	{
		std::vector<std::string> standards = m_sourceGroupSettings->getAvailableCStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(i, standards[i].c_str());
		}

		m_standard->setCurrentText(QString::fromStdString(m_sourceGroupSettings->getCStandard()));
	}
}

void QtProjectWizzardContentCStandard::save()
{
	if (m_sourceGroupSettings)
	{
		m_sourceGroupSettings->setCStandard(m_standard->currentText().toStdString());
	}
}
