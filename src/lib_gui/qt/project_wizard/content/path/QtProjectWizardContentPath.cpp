#include "QtProjectWizardContentPath.h"

#include <QMessageBox>

#include "SourceGroupSettings.h"
#include "utilityFile.h"

QtProjectWizardContentPath::QtProjectWizardContentPath(QtProjectWizardWindow* window)
	: QtProjectWizardContent(window)
{
}

void QtProjectWizardContentPath::populate(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel(m_titleString);
	layout->addWidget(label, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);

	if (m_helpString.size() > 0)
	{
		addHelpButton(m_titleString, m_helpString, layout, row);
	}

	m_picker = new QtLocationPicker(this);
	m_picker->setPickDirectory(true);
	m_picker->setPlaceholderText(m_placeholderString);

	m_picker->setRelativeRootDirectory(getSourceGroupSettings()->getProjectDirectoryPath());

	layout->addWidget(m_picker, row, QtProjectWizardWindow::BACK_COL);
	row++;
}

bool QtProjectWizardContentPath::check()
{
	QString error;

	while (true)
	{
		if (m_picker->getText().isEmpty())
		{
			if (!isRequired())
			{
				break;
			}

			error = "Please define a path at \"" + m_titleString + "\".";
			break;
		}

		FilePath path = utility::getExpandedAndAbsolutePath(
			FilePath(m_picker->getText().toStdWString()),
			getSourceGroupSettings()->getProjectDirectoryPath());

		if (m_picker->pickDirectory())
		{
			break;
		}

		if (!path.exists())
		{
			error = "The entered path does not exist at \"" + m_titleString + "\".";
			break;
		}

		if (!m_fileEndings.empty() && m_fileEndings.find(path.extension()) == m_fileEndings.end())
		{
			error = "The entered path does have a correct file ending at \"" + m_titleString + "\".";
			break;
		}

		break;
	}

	if (!error.isEmpty())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(error);
		msgBox.exec();
		return false;
	}

	return true;
}

void QtProjectWizardContentPath::setTitleString(const QString& title)
{
	m_titleString = title;
}

void QtProjectWizardContentPath::setHelpString(const QString& help)
{
	m_helpString = help;
}

void QtProjectWizardContentPath::setPlaceholderString(const QString& placeholder)
{
	m_placeholderString = placeholder;
}

void QtProjectWizardContentPath::setFileEndings(const std::set<std::wstring>& fileEndings)
{
	m_fileEndings = fileEndings;
}
