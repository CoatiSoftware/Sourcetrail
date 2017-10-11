#include "qt/window/project_wizzard/QtProjectWizzardContentCDBSource.h"

#include "data/indexer/IndexerCommandCxxCdb.h"
#include "settings/SourceGroupSettingsCxxCdb.h"
#include "utility/file/FilePath.h"

QtProjectWizzardContentCDBSource::QtProjectWizzardContentCDBSource(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_text(nullptr)
{
}

void QtProjectWizzardContentCDBSource::populate(QGridLayout* layout, int& row)
{
	QLabel* title = createFormLabel("Source Files");
	layout->addWidget(title, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	m_text = new QLabel("");
	m_text->setWordWrap(true);
	layout->addWidget(m_text, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignTop);

	addFilesButton("show source files", layout, row + 1);

	row += 2;
}

void QtProjectWizzardContentCDBSource::load()
{
	m_fileNames.clear();

	const FilePath projectPath = m_settings->getProjectDirectoryPath();
	std::vector<FilePath> excludePaths = m_settings->getExcludePathsExpandedAndAbsolute();

	if (std::shared_ptr<SourceGroupSettingsCxxCdb> cxxSettings = std::dynamic_pointer_cast<SourceGroupSettingsCxxCdb>(m_settings))
	{
		FilePath cdbPath = cxxSettings->getCompilationDatabasePathExpandedAndAbsolute();
		if (!cdbPath.empty() && cdbPath.exists())
		{
			std::vector<FilePath> filePaths = IndexerCommandCxxCdb::getSourceFilesFromCDB(cdbPath);

			for (FilePath& path : filePaths)
			{
				bool excluded = false;
				for (const FilePath& p : excludePaths)
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

				m_fileNames.push_back(path.str());
			}
		}
	}

	if (m_text)
	{
		m_text->setText("<b>" + QString::number(m_fileNames.size()) + "</b> source files were found in the compilation database.");
	}
}

std::vector<std::string> QtProjectWizzardContentCDBSource::getFileNames() const
{
	const_cast<QtProjectWizzardContentCDBSource*>(this)->load();

	return m_fileNames;
}

QString QtProjectWizzardContentCDBSource::getFileNamesTitle() const
{
	return "Source Files";
}

QString QtProjectWizzardContentCDBSource::getFileNamesDescription() const
{
	return " source files will be indexed.";
}
