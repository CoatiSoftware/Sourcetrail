#ifndef QT_BOOKMARK_CREATOR_H
#define QT_BOOKMARK_CREATOR_H

#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QWidget>

#include "qt/window/QtWindow.h"

#include "utility/types.h"

class BookmarkCategory;

class QtBookmarkCreator
	: public QtWindow
{
	Q_OBJECT

public:
	QtBookmarkCreator(QWidget* parent = nullptr, bool edit = false, Id id = 0);
	~QtBookmarkCreator();

	void setupBookmarkCreator();

	void refreshStyle();

	void setDisplayName(const std::string& name);
	void setComment(const std::string& comment);

	void setBookmarkCategories(const std::vector<BookmarkCategory>& categories);
	void setCurrentBookmarkCategory(const BookmarkCategory& category);

	bool getIsEdge() const;
	void setIsEdge(const bool isEdge);

protected:
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;

	virtual void handleNext() override;
	virtual void handleClose() override;

private slots:
	void onNameChanged(const QString& text);
	void onComboBoxIndexChanged(int index);

private:
	bool m_edit;
	Id m_bookmarkId; // important for editing

	QLineEdit* m_displayName;
	QTextEdit* m_commentBox;
	QComboBox* m_categoryBox;

	int m_categoryCount;

	bool m_isEdge;

	QWidget* m_headerBackground;
};

#endif // QT_BOOKMARK_CREATOR_H
