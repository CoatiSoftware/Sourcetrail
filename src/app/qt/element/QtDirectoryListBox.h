#ifndef QT_DIRECTORYLISTBOX_H
#define QT_DIRECTORYLISTBOX_H

#include <QFrame>
#include <QListWidget>
#include <QPushButton>
#include <QtGui/qevent.h>

#include "utility/file/FilePath.h"

class QtDirectoryListBox : public QFrame
{
Q_OBJECT
public:
	QtDirectoryListBox(QWidget *parent);
	std::vector<FilePath> getList();
protected:
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent* event);
private:
	QPushButton* m_addButton;
	QPushButton* m_removeButton;
	QListWidget* m_list;

private slots:
	void addListBoxItem();
	void removeListBoxItem();
};

#endif //QT_DIRECTORYLISTBOX_H
