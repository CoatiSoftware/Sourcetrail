#include "qt/window/project_wizzard/QtProjectWizzardContentJavaStandard.h"

#include <QComboBox>
#include <QLabel>

#include "settings/SourceGroupSettingsWithJavaStandard.h"

QtProjectWizzardContentJavaStandard::QtProjectWizzardContentJavaStandard(
	std::shared_ptr<SourceGroupSettingsWithJavaStandard> sourceGroupSettings,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_sourceGroupSettings(sourceGroupSettings)
	, m_standard(nullptr)
{
}

void QtProjectWizzardContentJavaStandard::populate(QGridLayout* layout, int& row)
{
	m_standard = new QComboBox();
	layout->addWidget(createFormLabel("Java Standard"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;
}

void QtProjectWizzardContentJavaStandard::load()
{
	m_standard->clear();

	if (m_sourceGroupSettings)
	{
		std::vector<std::string> standards = m_sourceGroupSettings->getAvailableJavaStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(i, standards[i].c_str());
		}

		m_standard->setCurrentText(QString::fromStdString(m_sourceGroupSettings->getJavaStandard()));
	}
}

void QtProjectWizzardContentJavaStandard::save()
{
	if (m_sourceGroupSettings)
	{
		m_sourceGroupSettings->setJavaStandard(m_standard->currentText().toStdString());
	}
}
