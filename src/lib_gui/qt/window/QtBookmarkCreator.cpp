#include "QtBookmarkCreator.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

#include "BookmarkCategory.h"
#include "MessageStatus.h"
#include "ResourcePaths.h"
#include "utilityQt.h"

QtBookmarkCreator::QtBookmarkCreator(
	ControllerProxy<BookmarkController>* controllerProxy, QWidget* parent, Id bookmarkId)
	: QtWindow(false, parent)
	, m_controllerProxy(controllerProxy)
	, m_editBookmarkId(bookmarkId)
	, m_nodeId(0)
{
}

QtBookmarkCreator::~QtBookmarkCreator() {}

void QtBookmarkCreator::setupBookmarkCreator()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	{
		// title
		QLabel* title = new QLabel(m_editBookmarkId ?
								QStringLiteral("Edit Bookmark") : QStringLiteral("Create Bookmark"));
		title->setObjectName(QStringLiteral("creator_title_label"));
		mainLayout->addWidget(title);
	}

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(20, 10, 20, 20);

	{
		// name
		QLabel* nameLabel = new QLabel(QStringLiteral("Name"));
		nameLabel->setObjectName(QStringLiteral("creator_label"));
		layout->addWidget(nameLabel);

		m_displayName = new QLineEdit();
		m_displayName->setObjectName(QStringLiteral("creator_name_edit"));
		m_displayName->setPlaceholderText(QStringLiteral("Name"));
		m_displayName->setAttribute(Qt::WA_MacShowFocusRect, 0);
		layout->addWidget(m_displayName);

		connect(m_displayName, &QLineEdit::textChanged, this, &QtBookmarkCreator::onNameChanged);

		layout->addSpacing(15);
	}

	{
		// comment
		QLabel* commentLabel = new QLabel(QStringLiteral("Comment"));
		commentLabel->setObjectName(QStringLiteral("creator_label"));
		layout->addWidget(commentLabel);

		m_commentBox = new QTextEdit();
		m_commentBox->setObjectName(QStringLiteral("creator_comment_box"));
		m_commentBox->setPlaceholderText(QStringLiteral("Comment"));
		layout->addWidget(m_commentBox);

		layout->addSpacing(15);
	}

	{
		// category
		QLabel* categoryLabel = new QLabel(QStringLiteral("Choose or Create Category"));
		categoryLabel->setObjectName(QStringLiteral("creator_label"));
		layout->addWidget(categoryLabel);

		m_categoryBox = new QComboBox();
		m_categoryBox->setObjectName(QStringLiteral("creator_category_box"));
		m_categoryBox->addItem(QLatin1String(""));
		m_categoryBox->setEditable(true);
		m_categoryBox->lineEdit()->setPlaceholderText(QStringLiteral("Category"));
		m_categoryBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAtTop);
		layout->addWidget(m_categoryBox);

		layout->addSpacing(20);
	}

	{
		layout->addLayout(createButtons());
		setPreviousVisible(false);
		updateNextButton(m_editBookmarkId ? QStringLiteral("Save") : QStringLiteral("Create"));
	}

	mainLayout->addLayout(layout);

	refreshStyle();
}

void QtBookmarkCreator::refreshStyle()
{
	setStyleSheet(
		(utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"window/window.css")) +
		 utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"bookmark_view/"
																		L"bookmark_view.css")))
			.c_str());
}

void QtBookmarkCreator::setDisplayName(const std::wstring& name)
{
	m_displayName->setText(QString::fromStdWString(name));
}

void QtBookmarkCreator::setComment(const std::wstring& comment)
{
	m_commentBox->setText(QString::fromStdWString(comment));
}

void QtBookmarkCreator::setBookmarkCategories(const std::vector<BookmarkCategory>& categories)
{
	for (unsigned int i = 0; i < categories.size(); i++)
	{
		m_categoryBox->addItem(QString::fromStdWString(categories[i].getName()));
	}
}

void QtBookmarkCreator::setCurrentBookmarkCategory(const BookmarkCategory& category)
{
	int index = m_categoryBox->findText(QString::fromStdWString(category.getName()));

	if (index > -1)
	{
		m_categoryBox->setCurrentIndex(index);
	}
	else
	{
		m_categoryBox->addItem(QString::fromStdWString(category.getName()));
		m_categoryBox->setCurrentIndex(1);
	}
}

void QtBookmarkCreator::setNodeId(Id nodeId)
{
	m_nodeId = nodeId;
}

void QtBookmarkCreator::handleNext()
{
	std::wstring name = m_displayName->text().toStdWString();
	std::wstring comment = m_commentBox->toPlainText().toStdWString();
	std::wstring category = m_categoryBox->currentText().toStdWString();

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
