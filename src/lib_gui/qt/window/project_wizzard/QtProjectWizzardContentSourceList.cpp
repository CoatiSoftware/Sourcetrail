#include "qt/window/project_wizzard/QtProjectWizzardContentSourceList.h"

#include <QListView>
#include <QStringListModel>

#include "utility/file/FileSystem.h"
#include "utility/utility.h"

QtProjectWizzardContentSourceList::QtProjectWizzardContentSourceList(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentSourceList::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);

	m_text = new QLabel("0 files will be analyzed.");
	m_text->setWordWrap(true);
	layout->addWidget(m_text);

	m_list = new QListView(this);
	m_list->setObjectName("files");
	m_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_list->setSelectionMode(QAbstractItemView::NoSelection);
	m_list->setAttribute(Qt::WA_MacShowFocusRect, 0);
	layout->addWidget(m_list);
}

void QtProjectWizzardContentSourceList::windowReady()
{
	m_window->updateTitle("Analyzed Files");
}

QSize QtProjectWizzardContentSourceList::preferredWindowSize() const
{
	return QSize(500, 500);
}

void QtProjectWizzardContentSourceList::showFilesFromSourcePaths()
{
	std::vector<FilePath> sourcePaths = m_settings->getSourcePaths();

	std::vector<std::string> extensions;
	utility::append(extensions, m_settings->getSourceExtensions());
	utility::append(extensions, m_settings->getHeaderExtensions());

	std::vector<FileInfo> fileInfos = FileSystem::getFileInfosFromPaths(sourcePaths, extensions);

	FilePath projectPath = FilePath(m_settings->getProjectFileLocation());

	QStringList list;
	for (const FileInfo& info : fileInfos)
	{
		FilePath path = info.path;

		if (projectPath.exists())
		{
			path = path.relativeTo(projectPath);
		}

		list << QString::fromStdString(path.str());
	}

	m_text->setText(QString::number(list.size()) + " files will be analyzed.");

    QStringListModel* model = new QStringListModel(this);
    model->setStringList(list);
    m_list->setModel(model);
}
