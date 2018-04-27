#ifndef QT_SELECT_PATHS_DIALOG_H
#define QT_SELECT_PATHS_DIALOG_H

#include "qt/window/QtTextEditDialog.h"

class FilePath;
class QListWidget;

class QtSelectPathsDialog
	: public QtTextEditDialog
{
	Q_OBJECT
public:
	QtSelectPathsDialog(const QString& title, const QString& description, QWidget* parent = 0);

	std::vector<FilePath> getPathsList() const;
	void setPathsList(const std::vector<FilePath>& paths, const std::vector<FilePath>& checkedPaths, const FilePath& rootPathForRelativePaths);

	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

private:
	QListWidget* m_list;

	void checkSelected(bool checked);
};

#endif // QT_SELECT_PATHS_DIALOG_H
