#ifndef QT_BOOKMARK_CREATOR_H
#define QT_BOOKMARK_CREATOR_H

#include <QWidget>

#include "component/controller/BookmarkController.h"
#include "component/controller/helper/ControllerProxy.h"
#include "qt/window/QtWindow.h"
#include "utility/types.h"

class BookmarkCategory;
class QComboBox;
class QLineEdit;
class QTextEdit;

class QtBookmarkCreator
	: public QtWindow
{
	Q_OBJECT

public:
	QtBookmarkCreator(ControllerProxy<BookmarkController>* controllerProxy, QWidget* parent = nullptr, Id bookmarkId = 0);
	~QtBookmarkCreator();

	void setupBookmarkCreator();

	void refreshStyle();

	void setDisplayName(const std::string& name);
	void setComment(const std::string& comment);

	void setBookmarkCategories(const std::vector<BookmarkCategory>& categories);
	void setCurrentBookmarkCategory(const BookmarkCategory& category);

	void setNodeId(Id nodeId);

protected:
	virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

	virtual void handleNext() override;
	virtual void handleClose() override;

private slots:
	void onNameChanged(const QString& text);

private:
	ControllerProxy<BookmarkController>* m_controllerProxy;

	const Id m_editBookmarkId;

	QLineEdit* m_displayName;
	QTextEdit* m_commentBox;
	QComboBox* m_categoryBox;

	Id m_nodeId;

	QWidget* m_headerBackground;
};

#endif // QT_BOOKMARK_CREATOR_H
