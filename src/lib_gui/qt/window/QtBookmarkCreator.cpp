#include "QtBookmarkCreator.h"

#include <QHBoxLayout>

#include "data/bookmark/BookmarkCategory.h"

#include "utility/messaging/type/MessageCreateBookmark.h"
#include "utility/messaging/type/MessageCreateBookmarkCategory.h"
#include "utility/messaging/type/MessageEditBookmark.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"

#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

QString QtBookmarkCreator::m_namePlaceholder = "Name";
QString QtBookmarkCreator::m_commentPlaceholder = "Comment";
QString QtBookmarkCreator::m_categoryPlaceholder = "Category";

QtBookmarkCreator::QtBookmarkCreator(QWidget* parent, bool edit, Id id)
	: QtWindow(parent)
	, m_edit(edit)
	, m_bookmarkId(id)
	, m_categoryCount(0)
{
}

QtBookmarkCreator::~QtBookmarkCreator()
{
}

void QtBookmarkCreator::setupBookmarkCreator()
{
	m_displayName = new QLineEdit(this);
	m_displayName->setObjectName("display_name_edit");
	m_commentBox = new QTextEdit(this);
	m_commentBox->setObjectName("comment_box");
	m_categoryBox = new QComboBox(this);
	m_categoryBox->setObjectName("category_box");

	m_title = new QLabel();
	if (m_edit == true)
	{
		m_title->setText("Edit Bookmark");
	}
	else
	{
		m_title->setText("Create Bookmark");
	}
	m_title->setObjectName("creator_title_label");
	m_nameLabel = new QLabel();
	m_nameLabel->setText("Bookmark:");
	m_nameLabel->setObjectName("creator_name_label");
	m_commentLabel = new QLabel();
	m_commentLabel->setText("Comment:");
	m_commentLabel->setObjectName("creator_comment_label");
	m_categoryLabel = new QLabel();
	m_categoryLabel->setText("Choose or Create Bookmark:");
	m_categoryLabel->setObjectName("category_label");

	m_categoryBox->addItem("");
	m_categoryBox->setEditable(true);
	m_categoryBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAtTop);

	m_categoryCount = m_categoryBox->count();

	m_cancelButton = new QPushButton(this);
	m_cancelButton->setObjectName("creator_cancel_button");
	m_cancelButton->setText("Cancel");
	m_createButton = new QPushButton(this);
	m_createButton->setObjectName("creator_create_button");
	if (m_edit)
	{
		m_createButton->setText("Save");
	}
	else
	{
		m_createButton->setText("Create");
	}

	m_commentBox->setPlaceholderText(m_commentPlaceholder);
	m_displayName->setPlaceholderText(m_namePlaceholder);
	m_categoryBox->lineEdit()->setPlaceholderText(m_categoryPlaceholder);

	m_createButton->setEnabled(false);

	connect(m_displayName, SIGNAL(textChanged(const QString&)), this, SLOT(onNameChanged(const QString&)));
	connect(m_commentBox, SIGNAL(textChanged()), this, SLOT(onCommentChanged()));
	connect(m_categoryBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onCategoryChanged(const QString&)));
	connect(m_categoryBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
	connect(m_createButton, SIGNAL(clicked()), this, SLOT(create()));

	m_layout = new QVBoxLayout(this);
	m_layout->addWidget(m_title);
	m_layout->addWidget(m_nameLabel);
	m_layout->addWidget(m_displayName);
	m_layout->addWidget(m_commentLabel);
	m_layout->addWidget(m_commentBox);
	m_layout->addWidget(m_categoryLabel);
	m_layout->addWidget(m_categoryBox);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);

	buttonLayout->addWidget(m_cancelButton);
	buttonLayout->setAlignment(m_cancelButton, Qt::AlignLeft);
	buttonLayout->addWidget(m_createButton);
	buttonLayout->setAlignment(m_createButton, Qt::AlignRight);

	m_layout->addLayout(buttonLayout);

	m_headerBackground = new QWidget(m_window);
	m_headerBackground->setObjectName("creator_header_background");
	m_headerBackground->setGeometry(0, 0, 0, 0);
	m_headerBackground->show();
	m_headerBackground->lower();

	setFixedSize(QSize(500, 580));
	refreshStyle();

	m_headerBackground->setGeometry(0, 0, size().width() - 10, 70);
}

void QtBookmarkCreator::refreshStyle()
{
	setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "bookmark_view/bookmark_view.css").c_str());
}

void QtBookmarkCreator::setDisplayName(const std::string& name)
{
	m_displayName->setText(name.c_str());
}

void QtBookmarkCreator::setComment(const std::string& comment)
{
	m_commentBox->setText(comment.c_str());
}

void QtBookmarkCreator::setBookmarkCategories(const std::vector<BookmarkCategory>& categories)
{
	for (unsigned int i = 0; i < categories.size(); i++)
	{
		m_categoryBox->addItem(categories[i].getName().c_str());
	}
}

void QtBookmarkCreator::setCurrentBookmarkCategory(const BookmarkCategory& category)
{
	int index = m_categoryBox->findText(category.getName().c_str());

	if (index > -1)
	{
		m_categoryBox->setCurrentIndex(index);
	}
	else
	{
		m_categoryBox->addItem(category.getName().c_str());
		m_categoryBox->setCurrentIndex(1);
	}
}

bool QtBookmarkCreator::getIsEdge() const
{
	return m_isEdge;
}

void QtBookmarkCreator::setIsEdge(const bool isEdge)
{
	m_isEdge = isEdge;
}

void QtBookmarkCreator::onNameChanged(const QString& text)
{
	if (text.length() > 0)
	{
		m_createButton->setEnabled(true);
	}
	else
	{
		m_createButton->setEnabled(false);
	}
}

void QtBookmarkCreator::onCommentChanged()
{
	/*if (m_commentBox->toPlainText().size() <= 0)
	{
		m_commentBox->
	}*/
}

void QtBookmarkCreator::onCategoryChanged(const QString& text)
{

}

void QtBookmarkCreator::onComboBoxIndexChanged(int index)
{
	if (m_categoryBox->count() > m_categoryCount)
	{
		std::string categoryName = m_categoryBox->currentText().toStdString();

		MessageCreateBookmarkCategory(categoryName).dispatch();

		m_categoryCount = m_categoryBox->count();
	}
}

void QtBookmarkCreator::cancel()
{
	close();
}

void QtBookmarkCreator::create()
{
	QString qComment = m_commentBox->toPlainText();
	QString qDisplayName = m_displayName->text();
	QString qCategory = m_categoryBox->currentText();

	if (m_edit)
	{
		MessageEditBookmark(m_bookmarkId, qComment.toStdString(), qDisplayName.toStdString(), qCategory.toStdString(), m_isEdge).dispatch();
	}
	else
	{
		MessageCreateBookmark(qComment.toStdString(), qDisplayName.toStdString(), qCategory.toStdString()).dispatch();
	}

	MessageStatus("Creating Bookmark for active Token").dispatch();

	close();
}
