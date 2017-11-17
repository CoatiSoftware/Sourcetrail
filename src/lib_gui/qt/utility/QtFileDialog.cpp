#include "qt/utility/QtFileDialog.h"

#include <QFileDialog>
#include <QListView>
#include <QTreeView>

#include "utility/utilityApp.h"

QStringList QtFileDialog::getFileNamesAndDirectories(QWidget* parent, const QString& dir)
{
	QFileDialog dialog(parent);

	if (utility::getOsType() != OS_MAC)
	{
		dialog.setFileMode(QFileDialog::Directory);
	}

	dialog.setDirectory(getDir(dir));

	QListView *l = dialog.findChild<QListView*>("listView");
	if (l)
	{
		l->setSelectionMode(QAbstractItemView::SingleSelection);
	}
	QTreeView *t = dialog.findChild<QTreeView*>();
	if (t)
	{
		t->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	QStringList list;

	if (dialog.exec())
	{
		list = dialog.selectedFiles();
	}

	return list;
}

QString QtFileDialog::getExistingDirectory(QWidget* parent, const QString& caption, const QString& dir)
{
	return QFileDialog::getExistingDirectory(parent, caption, getDir(dir));
}

QString QtFileDialog::getOpenFileName(QWidget* parent, const QString& caption, const QString& dir, const QString& filter)
{
	return QFileDialog::getOpenFileName(parent, caption, getDir(dir), filter);
}

QString QtFileDialog::showSaveFileDialog(
	QWidget *parent, const QString& title, const QString& directory, const QString& filter)
{
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)

	return QFileDialog::getSaveFileName(parent, title, directory, filter);

#else
	QFileDialog dialog(parent, title, directory, filter);

	if (parent)
	{
		dialog.setWindowModality(Qt::WindowModal);
	}

	QRegExp filter_regex(QLatin1String("(?:^\\*\\.(?!.*\\()|\\(\\*\\.)(\\w+)"));
	QStringList filters = filter.split(QLatin1String(";;"));

	if (!filters.isEmpty())
	{
		dialog.setNameFilters(filters);
	}

	dialog.setAcceptMode(QFileDialog::AcceptSave);

	if (dialog.exec() == QDialog::Accepted)
	{
		QString file_name = dialog.selectedFiles().constFirst();
		QFileInfo info(file_name);

		if (info.suffix().isEmpty() && !dialog.selectedNameFilter().isEmpty())
		{
			if (filter_regex.indexIn(dialog.selectedNameFilter()) != -1)
			{
				QString extension = filter_regex.cap(1);
				file_name += QLatin1String(".") + extension;
			}
		}
		return file_name;
	}
	else
	{
		return QString();
	}
#endif  // Q_OS_MAC || Q_OS_WIN
}

QString QtFileDialog::getDir(QString dir)
{
	static bool used = false;

	if (!used && !dir.size())
	{
		dir = QDir::homePath();
	}

	used = true;

	return dir;
}
