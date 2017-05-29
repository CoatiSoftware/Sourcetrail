#include "qt/window/project_wizzard/QtProjectWizzardContentLanguageAndStandard.h"

#include <QComboBox>
#include <QLabel>

#include "settings/SourceGroupSettings.h"
#include "utility/logging/logging.h"

QtProjectWizzardContentLanguageAndStandard::QtProjectWizzardContentLanguageAndStandard(
	std::shared_ptr<SourceGroupSettings> sourceGroupSettings,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_sourceGroupSettings(sourceGroupSettings)
	, m_language(nullptr)
	, m_standard(nullptr)
{
}

void QtProjectWizzardContentLanguageAndStandard::populate(QGridLayout* layout, int& row)
{
	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	m_language = new QLabel();
	layout->addWidget(createFormLabel("Language"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_language, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	m_standard = new QComboBox();
	layout->addWidget(createFormLabel("Standard"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_standard, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizzardContentLanguageAndStandard::load()
{
	LanguageType type = getLanguageTypeForSourceGroupType(m_sourceGroupSettings->getType());

	if (type == LANGUAGE_UNKNOWN)
	{
		LOG_ERROR("No language type defined");
		return;
	}

	m_language->setText(languageTypeToString(type).c_str());

	m_standard->clear();
	std::vector<std::string> standards = m_sourceGroupSettings->getAvailableLanguageStandards();
	for (size_t i = 0; i < standards.size(); i++)
	{
		m_standard->insertItem(i, standards[i].c_str());
	}

	m_standard->setCurrentText(QString::fromStdString(m_sourceGroupSettings->getStandard()));
}

void QtProjectWizzardContentLanguageAndStandard::save()
{
	m_sourceGroupSettings->setStandard(m_standard->currentText().toStdString());
}

bool QtProjectWizzardContentLanguageAndStandard::check()
{
	return true;
}
