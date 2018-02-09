#ifndef QT_DIRECTORY_LIST_BOX_H
#define QT_DIRECTORY_LIST_BOX_H

#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QtGui/qevent.h>

#include "utility/file/FilePath.h"

#include "qt/element/QtLineEdit.h"

class QtDirectoryListBox;
class QtTextEditDialog;

class QtListItemWidget
	: public QWidget
{
	Q_OBJECT

public:
	QtListItemWidget(QtDirectoryListBox* list, QListWidgetItem* item, QWidget *parent = nullptr);

	QString getText();
	void setText(QString text);

	bool readOnly() const;
	void setReadOnly(bool readOnly);

public slots:
	void setFocus();

private slots:
	void handleButtonPress();
	void handleFocus();

private:
	QPushButton* m_button;
	QtLineEdit* m_data;

	QtDirectoryListBox* m_list;
	QListWidgetItem* m_item;

	bool m_readOnly;
};


class QtDirectoryListBox
	: public QFrame
{
	Q_OBJECT

public:
	QtDirectoryListBox(QWidget *parent, const QString& listName, bool forStrings = false);

	void clear();

	std::vector<FilePath> getList();
	void setList(const std::vector<FilePath>& list, bool readOnly = false);

	std::vector<std::wstring> getStringList();
	void setStringList(const std::vector<std::wstring>& list, bool readOnly = false);

	QtListItemWidget* addListBoxItemWithText(const QString& text);

	void selectItem(QListWidgetItem* item);

	bool isForStrings() const;

	const FilePath& getRelativeRootDirectory() const;
	void setRelativeRootDirectory(const FilePath& dir);

protected:
	bool event(QEvent* event) override;

	void dropEvent(QDropEvent *event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;

private slots:
	QtListItemWidget* addListBoxItem();
	void removeListBoxItem();

	void showEditDialog();
	void canceledEditDialog();
	void savedEditDialog();

private:
	QPushButton* m_addButton;
	QPushButton* m_removeButton;
	QListWidget* m_list;

	QString m_listName;
	bool m_forStrings;

	std::shared_ptr<QtTextEditDialog> m_editDialog;

	FilePath m_relativeRootDirectory;
};

#endif // QT_DIRECTORY_LIST_BOX_H
