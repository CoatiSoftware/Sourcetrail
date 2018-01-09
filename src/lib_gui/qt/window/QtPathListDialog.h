#ifndef QT_PATH_LIST_DIALOG_H
#define QT_PATH_LIST_DIALOG_H

#include "qt/window/QtWindow.h"

class FilePath;
class QtDirectoryListBox;

class QtPathListDialog
	: public QtWindow
{
	Q_OBJECT

public:
	QtPathListDialog(const QString& title, const QString& description, QWidget* parent = 0);

	QSize sizeHint() const override;

	void setRelativeRootDirectory(const FilePath& dir);
	void setPaths(const std::vector<FilePath>& paths, bool readOnly = false);
	std::vector<FilePath> getPaths();

protected:
	void populateWindow(QWidget* widget) override;
	void windowReady() override;

	QString m_title;
	QString m_description;

private:
	QtDirectoryListBox* m_pathList;
};

#endif // QT_PATH_LIST_DIALOG_H
