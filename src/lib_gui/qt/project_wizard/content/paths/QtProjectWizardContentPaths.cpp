#include "QtProjectWizardContentPaths.h"

#include <QComboBox>
#include <QMessageBox>

#include "SourceGroupSettings.h"
#include "utility.h"

QtProjectWizardContentPaths::QtProjectWizardContentPaths(
	std::shared_ptr<SourceGroupSettings> settings,
	QtProjectWizardWindow* window,
	QtPathListBox::SelectionPolicyType selectionPolicy,
	bool checkMissingPaths)
	: QtProjectWizardContent(window)
	, m_settings(settings)
	, m_makePathsRelativeToProjectFileLocation(true)
	, m_selectionPolicy(selectionPolicy)
	, m_checkMissingPaths(checkMissingPaths)
{
}

void QtProjectWizardContentPaths::populate(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel(m_titleString);
	layout->addWidget(label, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);

	if (m_helpString.size() > 0)
	{
		addHelpButton(m_titleString, m_helpString, layout, row);
	}

	m_list = new QtPathListBox(this, m_titleString, m_selectionPolicy);

	if (m_makePathsRelativeToProjectFileLocation && m_settings)
	{
		m_list->setRelativeRootDirectory(m_settings->getProjectDirectoryPath());
	}

	layout->addWidget(m_list, row, QtProjectWizardWindow::BACK_COL);
	row++;

	if (m_showFilesString.size() > 0)
	{
		addFilesButton(m_showFilesString, layout, row);
		row++;
	}

	if (m_pathDetector)
	{
		addDetection(layout, row);
		row++;
	}
}

bool QtProjectWizardContentPaths::check()
{
	if (m_checkMissingPaths)
	{
		QString missingPaths;
		std::vector<FilePath> existingPaths;

		for (const FilePath& path: m_list->getPathsAsDisplayed())
		{
			std::vector<FilePath> expandedPaths(1, path);
			if (m_settings)
			{
				expandedPaths = m_settings->makePathsExpandedAndAbsolute(expandedPaths);
			}

			size_t existingCount = 0;
			for (const FilePath& expandedPath: expandedPaths)
			{
				if (!expandedPath.exists())
				{
					missingPaths.append(QString::fromStdWString(expandedPath.wstr() + L"\n"));
				}
				else
				{
					existingCount++;
				}
			}

			if (!expandedPaths.empty() && expandedPaths.size() == existingCount)
			{
				existingPaths.push_back(path);
			}
		}

		if (!missingPaths.isEmpty())
		{
			QMessageBox msgBox(m_window);
			msgBox.setText(QString("Some provided paths do not exist at \"%1\". Do you want to "
								   "remove them before continuing?")
							   .arg(m_titleString));
			msgBox.setDetailedText(missingPaths);
			QPushButton* removeButton = msgBox.addButton(
				QStringLiteral("Remove"), QMessageBox::YesRole);
			QPushButton* keepButton = msgBox.addButton(
				QStringLiteral("Keep"), QMessageBox::ButtonRole::NoRole);
			QPushButton* cancelButton = msgBox.addButton(
				QStringLiteral("Cancel"), QMessageBox::ButtonRole::RejectRole);

			msgBox.exec();

			if (msgBox.clickedButton() == removeButton)
			{
				m_list->setPaths(existingPaths);
				save();
			}
			else if (msgBox.clickedButton() == keepButton)
			{
				return true;
			}
			else if (msgBox.clickedButton() == cancelButton)
			{
				return false;
			}
		}
	}
	return true;
}

void QtProjectWizardContentPaths::setTitleString(const QString& title)
{
	m_titleString = title;
}

void QtProjectWizardContentPaths::setHelpString(const QString& help)
{
	m_helpString = help;
}

void QtProjectWizardContentPaths::addDetection(QGridLayout* layout, int row)
{
	std::vector<std::string> detectorNames = m_pathDetector->getWorkingDetectorNames();
	if (!detectorNames.size())
	{
		return;
	}

	QLabel* label = new QLabel(QStringLiteral("Auto detection from:"));

	m_detectorBox = new QComboBox();

	for (const std::string& detectorName: detectorNames)
	{
		m_detectorBox->addItem(detectorName.c_str());
	}

	QPushButton* button = new QPushButton(QStringLiteral("detect"));
	button->setObjectName(QStringLiteral("windowButton"));
	connect(button, &QPushButton::clicked, this, &QtProjectWizardContentPaths::detectionClicked);

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(label);
	hlayout->addWidget(m_detectorBox);
	hlayout->addWidget(button);

	QWidget* detectionWidget = new QWidget();
	detectionWidget->setLayout(hlayout);

	layout->addWidget(
		detectionWidget, row, QtProjectWizardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);
}

void QtProjectWizardContentPaths::detectionClicked()
{
	std::vector<FilePath> paths = m_pathDetector->getPathsForDetector(
		m_detectorBox->currentText().toStdString());
	std::vector<FilePath> oldPaths = m_list->getPathsAsDisplayed();

	paths = utility::unique(utility::concat(oldPaths, paths));

	detectedPaths(paths);
}

void QtProjectWizardContentPaths::detectedPaths(const std::vector<FilePath>& paths)
{
	m_list->setPaths(paths);
}
