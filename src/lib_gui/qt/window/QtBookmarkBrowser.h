#ifndef QT_BOOKMARK_BROWSER_H
#define QT_BOOKMARK_BROWSER_H

#include <QComboBox>
#include <QListWidget>
#include <QTreeWidget>

#include "Bookmark.h"
#include "BookmarkController.h"
#include "ControllerProxy.h"
#include "QtWindow.h"

class QtBookmark;

class QtBookmarkBrowser: public QtWindow
{
	Q_OBJECT

public:
	QtBookmarkBrowser(ControllerProxy<BookmarkController>* controllerProxy, QWidget* parent = nullptr);
	~QtBookmarkBrowser();

	void setupBookmarkBrowser();
	void setBookmarks(const std::vector<std::shared_ptr<Bookmark>>& bookmarks);

protected:
	virtual void resizeEvent(QResizeEvent* event) override;

	virtual void handleClose() override;
	virtual void handleNext() override;

private slots:
	void filterOrOrderChanged(const QString& text);
	void treeItemClicked(QTreeWidgetItem* item, int column);

private:
	Bookmark::BookmarkFilter getSelectedFilter();
	Bookmark::BookmarkOrder getSelectedOrder();

	QTreeWidgetItem* findOrCreateTreeCategory(const BookmarkCategory& category);

	ControllerProxy<BookmarkController>* m_controllerProxy;

	QTreeWidget* m_bookmarkTree;

	QComboBox* m_filterComboBox;
	QComboBox* m_orderComboBox;
	std::vector<std::string> m_orderNames;

	QWidget* m_headerBackground;
};

#endif	  // QT_BOOKMARK_BROWSER_H
