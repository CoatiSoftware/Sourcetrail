#ifndef QT_SELECT_PATHS_DIALOG_H
#define QT_SELECT_PATHS_DIALOG_H

#include "qt/window/QtTextEditDialog.h"
#include "utility/file/FilePath.h"

class QListWidget;

class QtSelectPathsDialog
	: public QtTextEditDialog
{
public:
	QtSelectPathsDialog(const QString& title, const QString& description, QWidget* parent = 0);

	std::vector<FilePath> getPathsList() const;
	void setPathsList(const std::vector<FilePath>& paths, const std::vector<FilePath>& checkedPaths);

	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

private:
	QListWidget* m_list;
};

#endif // QT_SELECT_PATHS_DIALOG_H
