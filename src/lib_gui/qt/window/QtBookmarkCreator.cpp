#include "QtBookmarkCreator.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

#include "data/bookmark/BookmarkCategory.h"
#include "qt/utility/utilityQt.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"

QtBookmarkCreator::QtBookmarkCreator(ControllerProxy<BookmarkController>* controllerProxy, QWidget* parent, Id bookmarkId)
    : QtWindow(false, parent)
    , m_controllerProxy(controllerProxy)
	, m_editBookmarkId(bookmarkId)
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
		QLabel* title = new QLabel(m_editBookmarkId ? "Edit Bookmark" : "Create Bookmark");
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

		connect(m_displayName, &QLineEdit::textChanged, this, &QtBookmarkCreator::onNameChanged);

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

		layout->addSpacing(20);
	}

	{
		layout->addLayout(createButtons());
		setPreviousVisible(false);
		updateNextButton(m_editBookmarkId ? "Save" : "Create");
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
		utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/window.css")) +
		utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"bookmark_view/bookmark_view.css"))
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
	std::string name = m_displayName->text().toStdString();
	std::string comment = m_commentBox->toPlainText().toStdString();
	std::string category = m_categoryBox->currentText().toStdString();

	if (m_editBookmarkId)
	{
		m_controllerProxy->executeAsTaskWithArgs(
			&BookmarkController::editBookmark, m_editBookmarkId, name, comment, category);
	}
	else
	{
		m_controllerProxy->executeAsTaskWithArgs(
			&BookmarkController::createBookmark, name, comment, category, m_nodeId);

		MessageStatus(L"Creating Bookmark for active Token").dispatch();
	}

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
