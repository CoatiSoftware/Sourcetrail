#include "QtProjectWizardContentFlags.h"

#include <QFormLayout>
#include <QMessageBox>

#include "QtStringListBox.h"
#include "SourceGroupSettingsWithCxxPathsAndFlags.h"

QtProjectWizardContentFlags::QtProjectWizardContentFlags(
	std::shared_ptr<SourceGroupSettingsWithCxxPathsAndFlags> settings,
	QtProjectWizardWindow* window,
	bool indicateAsAdditional)
	: QtProjectWizardContent(window)
	, m_settings(settings)
	, m_indicateAsAdditional(indicateAsAdditional)
{
}

void QtProjectWizardContentFlags::populate(QGridLayout* layout, int& row)
{
	const QString labelText(
		(std::string(m_indicateAsAdditional ? "Additional " : "") + "Compiler Flags").c_str());
	QLabel* label = createFormLabel(labelText);
	layout->addWidget(label, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);

	addHelpButton(
		labelText,
		QStringLiteral(
			"<p>Define additional Clang compiler flags used during indexing. Here are some "
			"examples:</p>"
			"<p>use \"-DRELEASE\" to add a preprocessor #define for \"RELEASE\"</p>"
			"<p>use \"-U__clang__\" to remove the preprocessor #define for \"__clang__\"</p>"),
		layout,
		row);

	m_list = new QtStringListBox(this, label->text());
	layout->addWidget(m_list, row, QtProjectWizardWindow::BACK_COL);
	row++;
}

void QtProjectWizardContentFlags::load()
{
	m_list->setStrings(m_settings->getCompilerFlags());
}

void QtProjectWizardContentFlags::save()
{
	m_settings->setCompilerFlags(m_list->getStrings());
}

bool QtProjectWizardContentFlags::check()
{
	std::wstring error;

	for (const std::wstring& flag: m_list->getStrings())
	{
		if (utility::isPrefix<std::wstring>(L"-include ", flag) ||
			utility::isPrefix<std::wstring>(L"--include ", flag))
		{
			error = L"The entered compiler flag \"" + flag +
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
