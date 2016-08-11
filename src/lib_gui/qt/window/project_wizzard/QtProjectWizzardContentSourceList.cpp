#include "qt/window/project_wizzard/QtProjectWizzardContentSourceList.h"

#include <QListView>
#include <QStringListModel>

QtProjectWizzardContentSourceList::QtProjectWizzardContentSourceList(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentSourceList::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);
	layout->setContentsMargins(0, 0, 0, 0);

	m_text = new QLabel("0 files will be indexed.");
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
}

QSize QtProjectWizzardContentSourceList::preferredWindowSize() const
{
	return QSize(500, 500);
}

void QtProjectWizzardContentSourceList::showFilesFromContent(QtProjectWizzardContent* content)
{
	QStringList list = content->getFileNames();
	m_text->setText(QString::number(list.size()) + " " + content->getFileNamesDescription());
	m_window->updateTitle(content->getFileNamesTitle());

	QStringListModel* model = new QStringListModel(this);
	model->setStringList(list);
	m_list->setModel(model);
}
