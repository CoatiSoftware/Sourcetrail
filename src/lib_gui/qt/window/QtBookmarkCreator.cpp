#include "QtBookmarkCreator.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "data/bookmark/BookmarkCategory.h"

#include "utility/messaging/type/MessageCreateBookmark.h"
#include "utility/messaging/type/MessageCreateBookmarkCategory.h"
#include "utility/messaging/type/MessageEditBookmark.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"

#include "qt/utility/utilityQt.h"

QtBookmarkCreator::QtBookmarkCreator(QWidget* parent, bool edit, Id id)
    : QtWindow(false, parent)
	, m_edit(edit)
	, m_bookmarkId(id)
	, m_categoryCount(0)
	, m_nodeId(0)
{
}

QtBookmarkCreator::~QtBookmarkCreator()
{
}

void QtBookmarkCreator::setupBookmarkCreator()
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(20, 23, 20, 20);

	{
		// title
		QLabel* title = new QLabel(m_edit ? "Edit Bookmark" : "Create Bookmark");
		title->setObjectName("creator_title_label");
		layout->addWidget(title);

		layout->addSpacing(30);
	}

	{
		// name
		QLabel* nameLabel = new QLabel("Bookmark:");
		nameLabel->setObjectName("creator_label");
		layout->addWidget(nameLabel);

		m_displayName = new QLineEdit();
		m_displayName->setObjectName("creator_name_edit");
		m_displayName->setPlaceholderText("Name");
		m_displayName->setAttribute(Qt::WA_MacShowFocusRect, 0);
		layout->addWidget(m_displayName);

		connect(m_displayName, SIGNAL(textChanged(const QString&)), this, SLOT(onNameChanged(const QString&)));

		layout->addSpacing(15);
	}

	{
		// comment
		QLabel* commentLabel = new QLabel("Comment:");
		commentLabel->setObjectName("creator_label");
		layout->addWidget(commentLabel);

		m_commentBox = new QTextEdit();
		m_commentBox->setObjectName("creator_comment_box");
		m_commentBox->setPlaceholderText("Comment");
		layout->addWidget(m_commentBox);

		layout->addSpacing(15);
	}

	{
		// category
		QLabel* categoryLabel = new QLabel("Choose or Create Category:");
		categoryLabel->setObjectName("creator_label");
		layout->addWidget(categoryLabel);

		m_categoryBox = new QComboBox();
		m_categoryBox->setObjectName("creator_category_box");
		m_categoryBox->addItem("");
		m_categoryBox->setEditable(true);
		m_categoryBox->lineEdit()->setPlaceholderText("Category");
		m_categoryBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAtTop);
		layout->addWidget(m_categoryBox);

		connect(m_categoryBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

		m_categoryCount = m_categoryBox->count();

		layout->addSpacing(20);
	}

	{
		layout->addLayout(createButtons());
		setPreviousVisible(false);
		updateNextButton(m_edit ? "Save" : "Create");
	}

	{
		// header
		m_headerBackground = new QWidget(m_window);
		m_headerBackground->setObjectName("creator_header_background");
		m_headerBackground->setGeometry(0, 0, 0, 0);
		m_headerBackground->show();
		m_headerBackground->lower();
	}

	refreshStyle();
}

void QtBookmarkCreator::refreshStyle()
{
	setStyleSheet((
		utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("window/window.css"))) +
		utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("bookmark_view/bookmark_view.css")))
	).c_str());
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

void QtBookmarkCreator::setNodeId(Id nodeId)
{
	m_nodeId = nodeId;
}

void QtBookmarkCreator::resizeEvent(QResizeEvent* event)
{
	QtWindow::resizeEvent(event);

	m_headerBackground->setGeometry(0, 0, m_window->size().width(), 60);
}

void QtBookmarkCreator::handleNext()
{
	QString qComment = m_commentBox->toPlainText();
	QString qDisplayName = m_displayName->text();
	QString qCategory = m_categoryBox->currentText();

	if (m_edit)
	{
		MessageEditBookmark(
			m_bookmarkId, qComment.toStdString(), qDisplayName.toStdString(), qCategory.toStdString()).dispatch();
	}
	else
	{
		MessageCreateBookmark(qComment.toStdString(), qDisplayName.toStdString(), qCategory.toStdString(), m_nodeId).dispatch();
	}

	MessageStatus("Creating Bookmark for active Token").dispatch();

	close();
}

void QtBookmarkCreator::handleClose()
{
	close();
}

void QtBookmarkCreator::onNameChanged(const QString& text)
{
	setNextEnabled(text.length() > 0);
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
