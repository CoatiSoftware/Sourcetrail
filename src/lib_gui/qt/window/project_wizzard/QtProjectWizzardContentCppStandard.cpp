#include "QtProjectWizzardContentCppStandard.h"

#include <QComboBox>
#include <QLabel>

#include "SourceGroupSettingsWithCppStandard.h"

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
	m_standard = new QComboBox();
	layout->addWidget(createFormLabel("C++ Standard"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;
}

void QtProjectWizzardContentCppStandard::load()
{
	m_standard->clear();

	if (m_sourceGroupSettings)
	{
		std::vector<std::wstring> standards = m_sourceGroupSettings->getAvailableCppStandards();
		for (size_t i = 0; i < standards.size(); i++)
		{
			m_standard->insertItem(i, QString::fromStdWString(standards[i]));
		}

		m_standard->setCurrentText(QString::fromStdWString(m_sourceGroupSettings->getCppStandard()));
	}
}

void QtProjectWizzardContentCppStandard::save()
{
	if (m_sourceGroupSettings)
	{
		m_sourceGroupSettings->setCppStandard(m_standard->currentText().toStdWString());
	}
}
