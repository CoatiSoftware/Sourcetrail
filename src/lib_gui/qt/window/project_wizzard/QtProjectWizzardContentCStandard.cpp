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
	m_standard = new QComboBox();
	layout->addWidget(createFormLabel("C Standard"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;
}

void QtProjectWizzardContentCStandard::load()
{
	m_standard->clear();

	if (m_sourceGroupSettings)
	{
		std::vector<std::wstring> standards = m_sourceGroupSettings->getAvailableCStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(i, QString::fromStdWString(standards[i]));
		}

		m_standard->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getCStandard()));
	}
}

void QtProjectWizzardContentCStandard::save()
{
	if (m_sourceGroupSettings)
	{
		m_sourceGroupSettings->setCStandard(m_standard->currentText().toStdWString());
	}
}
