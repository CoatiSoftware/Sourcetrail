#include "qt/window/project_wizzard/QtProjectWizzardContentFlags.h"

#include <QFormLayout>
#include <QMessageBox>

#include "qt/element/QtStringListBox.h"
#include "settings/SourceGroupSettingsCxx.h"

QtProjectWizzardContentFlags::QtProjectWizzardContentFlags(std::shared_ptr<SourceGroupSettingsCxx> settings, QtProjectWizzardWindow* window, bool indicateAsAdditional)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_indicateAsAdditional(indicateAsAdditional)
{
}

void QtProjectWizzardContentFlags::populate(QGridLayout* layout, int& row)
{
	const QString labelText((std::string(m_indicateAsAdditional ? "Additional " : "") + "Compiler Flags").c_str());
	QLabel* label = createFormLabel(labelText);
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);

	addHelpButton(
		labelText, 
		"<p>Define additional Clang compiler flags used during indexing. Here are some examples:</p>"
		"<p>use \"-DRELEASE\" to add a preprocessor #define for \"RELEASE\"</p>"
		"<p>use \"-U__clang__\" to remove the preprocessor #define for \"__clang__\"</p>",
		layout, 
		row
	);

	m_list = new QtStringListBox(this, label->text());
	layout->addWidget(m_list, row, QtProjectWizzardWindow::BACK_COL);
	row++;
}

void QtProjectWizzardContentFlags::load()
{
	m_list->setStrings(m_settings->getCompilerFlags());
}

void QtProjectWizzardContentFlags::save()
{
	m_settings->setCompilerFlags(m_list->getStrings());
}

bool QtProjectWizzardContentFlags::check()
{
	std::wstring error;

	for (const std::wstring& flag : m_list->getStrings())
	{
		if (utility::isPrefix<std::wstring>(L"-include ", flag) || utility::isPrefix<std::wstring>(L"--include ", flag))
		{
			error = L"The entered compiler flag \"" + flag + L"\" contains an error. Please remove the intermediate space character.\n";
		}
	}

	if (!error.empty())
	{
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdWString(error));
		msgBox.exec();
		return false;
	}

	return true;
}
