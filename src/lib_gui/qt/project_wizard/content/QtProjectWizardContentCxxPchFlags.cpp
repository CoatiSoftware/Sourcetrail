#include "QtProjectWizardContentCxxPchFlags.h"

#include <QCheckBox>
#include <QMessageBox>

#include "QtStringListBox.h"
#include "SourceGroupSettingsWithCxxPchOptions.h"

QtProjectWizardContentCxxPchFlags::QtProjectWizardContentCxxPchFlags(
	std::shared_ptr<SourceGroupSettingsWithCxxPchOptions> settings,
	QtProjectWizardWindow* window,
	bool isCDB)
	: QtProjectWizardContent(window), m_settings(settings), m_isCDB(isCDB)
{
}

void QtProjectWizardContentCxxPchFlags::populate(QGridLayout* layout, int& row)
{
	const QString labelText("Precompiled Header Flags");
	layout->addWidget(
		createFormLabel(labelText), row, QtProjectWizardWindow::FRONT_COL, 2, 1, Qt::AlignTop);

	const QString optionText(
		m_isCDB ? "Use flags of first indexed file and 'Additional Compiler Flags'"
				: "Use 'Compiler Flags'");

	const QString optionHelp(
		m_isCDB
			? "Check <b>" + optionText +
				"</b> to use the flags specified "
				"in the first compile command of the Compilation Database and all flags specified "
				"at 'Additional Compiler Flags'."
			: "Check <b>" + optionText + "</b> to reuse the flags specified at 'Compiler Flags'.");

	addHelpButton(
		"Precompiled Header Flags",
		"<p>Define compiler flags used during precompiled header file generation.</p>"
		"<p>" +
			optionHelp +
			"</p>"
			"<p>Additionally add compiler flags to the list for precompiled header generation "
			"only. Some examples:</p>"
			"<p>* use \"-DRELEASE\" to add a preprocessor #define for \"RELEASE\"</p>"
			"<p>* use \"-U__clang__\" to remove the preprocessor #define for \"__clang__\"</p>",
		layout,
		row);

	m_useCompilerFlags = new QCheckBox(optionText);
	layout->addWidget(m_useCompilerFlags, row, QtProjectWizardWindow::BACK_COL);
	row++;

	m_list = new QtStringListBox(this, labelText);
	layout->addWidget(m_list, row, QtProjectWizardWindow::BACK_COL);
	row++;
}

void QtProjectWizardContentCxxPchFlags::load()
{
	m_useCompilerFlags->setChecked(m_settings->getUseCompilerFlags());
	m_list->setStrings(m_settings->getPchFlags());
}

void QtProjectWizardContentCxxPchFlags::save()
{
	m_settings->setUseCompilerFlags(m_useCompilerFlags->isChecked());
	m_settings->setPchFlags(m_list->getStrings());
}

bool QtProjectWizardContentCxxPchFlags::check()
{
	std::wstring error;

	for (const std::wstring& flag: m_list->getStrings())
	{
		if (utility::isPrefix<std::wstring>(L"-include ", flag) ||
			utility::isPrefix<std::wstring>(L"--include ", flag))
		{
			error = L"The entered flag \"" + flag +
				L"\" contains an error. Please remove the intermediate space character.\n";
		}
	}

	if (!error.empty())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(QString::fromStdWString(error));
		msgBox.exec();
		return false;
	}

	return true;
}
