#include "qt/element/QtCodeFileList.h"

#include <QPropertyAnimation>
#include <QScrollBar>
#include <QVariant>
#include <QVBoxLayout>

#include "utility/file/FileSystem.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"

QtCodeFileList::QtCodeFileList(QWidget* parent)
	: QScrollArea(parent)
	, m_focusedTokenId(0)
{
	setObjectName("code_file_list_base");

	m_frame = std::make_shared<QFrame>(this);
	m_frame->setObjectName("code_file_list");

	QVBoxLayout* layout = new QVBoxLayout(m_frame.get());
	layout->setSpacing(8);
	layout->setContentsMargins(8, 8, 8, 8);
	layout->setAlignment(Qt::AlignTop);
	m_frame->setLayout(layout);

	setWidgetResizable(true);
	setWidget(m_frame.get());

	connect(this, SIGNAL(shouldScrollToSnippet(QWidget*)), this, SLOT(scrollToSnippet(QWidget*)), Qt::QueuedConnection);
}

QtCodeFileList::~QtCodeFileList()
{
}

QSize QtCodeFileList::sizeHint() const
{
	return QSize(800, 800);
}

void QtCodeFileList::addCodeSnippet(
	uint startLineNumber,
	const std::string& title,
	Id titleId,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile,
	int refCount,
	bool insert
){
	QtCodeFile* file = getFile(locationFile);

	if (insert)
	{
		QWidget* snippet = file->insertCodeSnippet(startLineNumber, title, titleId, code, locationFile, refCount);
		emit shouldScrollToSnippet(snippet);
	}
	else
	{
		file->addCodeSnippet(startLineNumber, title, titleId, code, locationFile, refCount);
	}
}

void QtCodeFileList::addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount)
{
	QtCodeFile* file = getFile(locationFile);
	file->setLocationFile(locationFile, refCount);
}

void QtCodeFileList::clearCodeSnippets()
{
	m_files.clear();
	this->verticalScrollBar()->setValue(0);
}

Id QtCodeFileList::getFocusedTokenId() const
{
	return m_focusedTokenId;
}

const std::vector<Id>& QtCodeFileList::getActiveTokenIds() const
{
	return m_activeTokenIds;
}

void QtCodeFileList::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_activeTokenIds = activeTokenIds;
}

const std::vector<std::string>& QtCodeFileList::getErrorMessages() const
{
	return m_errorMessages;
}

void QtCodeFileList::setErrorMessages(const std::vector<std::string>& errorMessages)
{
	m_errorMessages = errorMessages;
}

bool QtCodeFileList::scrollToFirstActiveSnippet()
{
	updateFiles();

	QWidget* widget = nullptr;
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		widget = file->findFirstActiveSnippet();
		if (widget)
		{
			if (!widget->isVisible())
			{
				file->clickedSnippetButton();
			}

			emit shouldScrollToSnippet(widget);
			return true;
		}
	}

	return false;
}

void QtCodeFileList::expandActiveSnippetFile()
{
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		if (file->openCollapsedActiveSnippet())
		{
			return;
		}
	}
}

void QtCodeFileList::focusToken(Id tokenId)
{
	m_focusedTokenId = tokenId;
	updateFiles();
}

void QtCodeFileList::defocusToken()
{
	m_focusedTokenId = 0;
	updateFiles();
}

void QtCodeFileList::scrollToSnippet(QWidget* widget)
{
	this->ensureWidgetVisibleAnimated(widget);
}

QtCodeFile* QtCodeFileList::getFile(std::shared_ptr<TokenLocationFile> locationFile)
{
	FilePath filePath = locationFile->getFilePath();
	QtCodeFile* file = nullptr;

	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		if (filePtr->getFilePath() == filePath)
		{
			file = filePtr.get();
			break;
		}
	}

	if (!file)
	{
		std::shared_ptr<QtCodeFile> filePtr = std::make_shared<QtCodeFile>(locationFile->getFilePath(), this);
		m_files.push_back(filePtr);

		file = filePtr.get();
		m_frame->layout()->addWidget(file);
	}

	return file;
}

void QtCodeFileList::updateFiles()
{
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		file->updateContent();
	}
}

void QtCodeFileList::ensureWidgetVisibleAnimated(QWidget *childWidget, int xmargin, int ymargin)
{
	if (!widget()->isAncestorOf(childWidget))
	{
		return;
	}

	const QRect microFocus = childWidget->inputMethodQuery(Qt::ImCursorRectangle).toRect();
	const QRect defaultMicroFocus = childWidget->QWidget::inputMethodQuery(Qt::ImCursorRectangle).toRect();
	QRect focusRect = (microFocus != defaultMicroFocus)
		? QRect(childWidget->mapTo(widget(), microFocus.topLeft()), microFocus.size())
		: QRect(childWidget->mapTo(widget(), QPoint(0, 0)), childWidget->size());
	const QRect visibleRect(-widget()->pos(), viewport()->size());

	if (visibleRect.contains(focusRect))
	{
		return;
	}

	focusRect.adjust(-xmargin, -ymargin, xmargin, ymargin);

	QScrollBar* scrollBar = nullptr;
	int value = 0;

	if (focusRect.width() > visibleRect.width())
	{
		scrollBar = horizontalScrollBar();
		value = focusRect.center().x() - viewport()->width() / 2;
	}
	else if (focusRect.right() > visibleRect.right())
	{
		scrollBar = horizontalScrollBar();
		value = focusRect.right() - viewport()->width();
	}
	else if (focusRect.left() < visibleRect.left())
	{
		scrollBar = horizontalScrollBar();
		value = focusRect.left();
	}

	if (focusRect.height() > visibleRect.height())
	{
		scrollBar = verticalScrollBar();
		value = focusRect.center().y() - viewport()->height() / 2;
	}
	else if (focusRect.bottom() > visibleRect.bottom())
	{
		scrollBar = verticalScrollBar();
		value = focusRect.bottom() - viewport()->height();
	}
	else if (focusRect.top() < visibleRect.top())
	{
		scrollBar = verticalScrollBar();
		value = focusRect.top();
	}

	if (scrollBar)
	{
		QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
		anim->setDuration(500);
		anim->setStartValue(scrollBar->value());
		anim->setEndValue(value);
		anim->setEasingCurve(QEasingCurve::InOutQuad);
		anim->start();
	}
}
