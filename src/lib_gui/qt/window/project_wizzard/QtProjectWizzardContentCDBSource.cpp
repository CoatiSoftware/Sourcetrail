#include "qt/window/project_wizzard/QtProjectWizzardContentCDBSource.h"

#include "data/parser/cxx/TaskParseCxx.h"

QtProjectWizzardContentCDBSource::QtProjectWizzardContentCDBSource(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
	, m_text(nullptr)
{
}

void QtProjectWizzardContentCDBSource::populateWindow(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 10);

	QLabel* title = new QLabel("Source Files");
	title->setWordWrap(true);
	title->setObjectName("section");
	layout->addWidget(title, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	m_text = new QLabel("");
	layout->addWidget(m_text, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignTop);

	addFilesButton("show source files", layout, row + 1);

	row += 2;

	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 2);
}

void QtProjectWizzardContentCDBSource::load()
{
	m_fileNames.clear();

	FilePath projectPath = m_settings->getProjectFileLocation();
	std::vector<FilePath> excludePaths = m_settings->getAbsoluteExcludePaths();

	std::vector<FilePath> filePaths = TaskParseCxx::getSourceFilesFromCDB(m_settings->getCompilationDatabasePath());
	for (FilePath path : filePaths)
	{
		bool excluded = false;
		for (FilePath p : excludePaths)
		{
			if (p == path || p.contains(path))
			{
				excluded = true;
				break;
			}
		}

		if (excluded)
		{
			continue;
		}

		if (projectPath.exists())
		{
			path = path.relativeTo(projectPath);
		}

		m_fileNames << QString::fromStdString(path.str());
	}

	if (m_text)
	{
		m_text->setText(QString::number(m_fileNames.size()) + " source files were found in the compilation database.");
	}
}

QStringList QtProjectWizzardContentCDBSource::getFileNames() const
{
	return m_fileNames;
}

QString QtProjectWizzardContentCDBSource::getFileNamesTitle() const
{
	return "Source Files";
}

QString QtProjectWizzardContentCDBSource::getFileNamesDescription() const
{
	return "source files will be analyzed.";
}
