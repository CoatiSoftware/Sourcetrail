#include "qt/window/project_wizzard/QtProjectWizzardContentCppStandard.h"

#include <QComboBox>
#include <QLabel>

#include "settings/SourceGroupSettingsWithCppStandard.h"

QtProjectWizzardContentCppStandard::QtProjectWizzardContentCppStandard(
	std::shared_ptr<SourceGroupSettingsWithCppStandard> sourceGroupSettings,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_sourceGroupSettings(sourceGroupSettings)
	, m_standard(nullptr)
{
}

void QtProjectWizzardContentCppStandard::populate(QGridLayout* layout, int& row)
{
	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	m_standard = new QComboBox();
	layout->addWidget(createFormLabel("C++ Standard"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizzardContentCppStandard::load()
{
	m_standard->clear();

	if (m_sourceGroupSettings)
	{
		std::vector<std::string> standards = m_sourceGroupSettings->getAvailableCppStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(i, standards[i].c_str());
		}

		m_standard->setCurrentText(QString::fromStdString(m_sourceGroupSettings->getCppStandard()));
	}
}

void QtProjectWizzardContentCppStandard::save()
{
	if (m_sourceGroupSettings)
	{
		m_sourceGroupSettings->setCppStandard(m_standard->currentText().toStdString());
	}
}
