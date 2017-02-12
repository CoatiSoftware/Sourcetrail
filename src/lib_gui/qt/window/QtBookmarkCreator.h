#ifndef QT_BOOKMARK_CREATOR_H
#define QT_BOOKMARK_CREATOR_H

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include <QVBoxLayout>

#include "qt/window/QtWindow.h"

#include "utility/types.h"

class BookmarkCategory;

class QtBookmarkCreator
	: public QtWindow
{
	Q_OBJECT

public:
	QtBookmarkCreator(QWidget* parent = nullptr, bool edit = false, Id id = -1);
	~QtBookmarkCreator();

	void setupBookmarkCreator();

	void refreshStyle();

	void setDisplayName(const std::string& name);
	void setComment(const std::string& comment);
	
	void setBookmarkCategories(const std::vector<BookmarkCategory>& categories);
	void setCurrentBookmarkCategory(const BookmarkCategory& category);

	bool getIsEdge() const;
	void setIsEdge(const bool isEdge);

private slots:
	void onNameChanged(const QString& text);
	void onCommentChanged();
	void onCategoryChanged(const QString& text);
	void onComboBoxIndexChanged(int index);

	void cancel();
	void create();

private:
	bool m_edit;
	Id m_bookmarkId; // important for editing

	QLabel* m_title;
	QLabel* m_nameLabel;
	QLabel* m_commentLabel;
	QLabel* m_categoryLabel;

	QLineEdit* m_displayName;
	QTextEdit* m_commentBox;
	QComboBox* m_categoryBox;

	QPushButton* m_cancelButton;
	QPushButton* m_createButton;

	QVBoxLayout* m_layout;

	int m_categoryCount;

	static QString m_namePlaceholder;
	static QString m_commentPlaceholder;
	static QString m_categoryPlaceholder;

	bool m_isEdge;

	QWidget* m_headerBackground;
};

#endif // QT_BOOKMARK_CREATOR_H