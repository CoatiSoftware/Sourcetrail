#include "qt/element/QtCodeFileList.h"

#include <QPropertyAnimation>
#include <QScrollBar>
#include <QTimer>
#include <QVariant>
#include <QVBoxLayout>

#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageScrollCode.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFileList::QtCodeFileList(QWidget* parent)
	: QScrollArea(parent)
	, m_scrollToFile(nullptr)
	, m_value(0)
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

	connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolled(int)));
	connect(this, SIGNAL(shouldScrollToSnippet(QtCodeSnippet*)), this, SLOT(scrollToSnippet(QtCodeSnippet*)), Qt::QueuedConnection);
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
	TimePoint modificationTime,
	bool insert
){
	QtCodeFile* file = getFile(locationFile->getFilePath());

	if (insert)
	{
		QtCodeSnippet* snippet = file->insertCodeSnippet(startLineNumber, title, titleId, code, locationFile, refCount);
		emit shouldScrollToSnippet(snippet);
	}
	else
	{
		file->addCodeSnippet(startLineNumber, title, titleId, code, locationFile, refCount);
	}
	file->setModificationTime(modificationTime);
}

void QtCodeFileList::addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount, TimePoint modificationTime)
{
	QtCodeFile* file = getFile(locationFile->getFilePath());
	file->setLocationFile(locationFile, refCount);
	file->setModificationTime(modificationTime);
}

void QtCodeFileList::clearCodeSnippets()
{
	m_files.clear();
	this->verticalScrollBar()->setValue(0);
}

const std::vector<Id>& QtCodeFileList::getActiveTokenIds() const
{
	return m_activeTokenIds;
}

void QtCodeFileList::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_activeTokenIds = activeTokenIds;
}

const std::vector<Id>& QtCodeFileList::getFocusedTokenIds() const
{
	return m_focusedTokenIds;
}

void QtCodeFileList::setFocusedTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_focusedTokenIds = focusedTokenIds;
}

const std::vector<std::string>& QtCodeFileList::getErrorMessages() const
{
	return m_errorMessages;
}

void QtCodeFileList::setErrorMessages(const std::vector<std::string>& errorMessages)
{
	m_errorMessages = errorMessages;
}

void QtCodeFileList::showActiveTokenIds()
{
	updateFiles();
}

void QtCodeFileList::showFirstActiveSnippet(bool scrollTo)
{
	updateFiles();

	QtCodeSnippet* snippet = getFirstActiveSnippet();

	if (!snippet)
	{
		expandActiveSnippetFile(scrollTo);
		return;
	}

	if (!snippet->isVisible())
	{
		snippet->getFile()->setSnippets();
	}

	if (scrollTo)
	{
		emit shouldScrollToSnippet(snippet);
	}
}

void QtCodeFileList::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	setFocusedTokenIds(focusedTokenIds);
	updateFiles();
}

void QtCodeFileList::defocusTokenIds()
{
	setFocusedTokenIds(std::vector<Id>());
	updateFiles();
}

void QtCodeFileList::setFileMinimized(const FilePath path)
{
	QtCodeFile* file = getFile(path);
	if (file)
	{
		file->setMinimized();
	}
}

void QtCodeFileList::setFileSnippets(const FilePath path)
{
	QtCodeFile* file = getFile(path);
	if (file)
	{
		file->setSnippets();
	}
}

void QtCodeFileList::setFileMaximized(const FilePath path)
{
	QtCodeFile* file = getFile(path);
	if (file)
	{
		file->setMaximized();
	}
}

void QtCodeFileList::showContents()
{
	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		filePtr->show();
	}
}

void QtCodeFileList::scrollToValue(int value)
{
	m_value = value;
	QTimer::singleShot(100, this, SLOT(setValue()));
}

void QtCodeFileList::scrollToActiveFileIfRequested()
{
	if (m_scrollToFile && m_scrollToFile->hasSnippets())
	{
		showFirstActiveSnippet(true);
		m_scrollToFile = nullptr;
	}
}

void QtCodeFileList::scrolled(int value)
{
	MessageScrollCode(value).dispatch();
}

void QtCodeFileList::scrollToSnippet(QtCodeSnippet* snippet)
{
	this->ensureWidgetVisibleAnimated(snippet, snippet->getFirstActiveLineRect());
}

void QtCodeFileList::setValue()
{
	this->verticalScrollBar()->setValue(m_value);
}

QtCodeFile* QtCodeFileList::getFile(const FilePath filePath)
{
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
		std::shared_ptr<QtCodeFile> filePtr = std::make_shared<QtCodeFile>(filePath, this);
		m_files.push_back(filePtr);

		file = filePtr.get();
		m_frame->layout()->addWidget(file);

		file->hide();
	}

	return file;
}

QtCodeSnippet* QtCodeFileList::getFirstActiveSnippet() const
{
	QtCodeSnippet* snippet = nullptr;
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		snippet = file->findFirstActiveSnippet();
		if (snippet)
		{
			break;
		}
	}

	return snippet;
}

void QtCodeFileList::updateFiles()
{
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		file->updateContent();
	}
}

void QtCodeFileList::expandActiveSnippetFile(bool scrollTo)
{
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		if (file->isCollapsedActiveFile())
		{
			file->requestSnippets();

			if (scrollTo)
			{
				m_scrollToFile = file.get();
			}

			return;
		}
	}
}

void QtCodeFileList::ensureWidgetVisibleAnimated(QWidget *childWidget, QRectF rect)
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

	if (rect.height() > 0)
	{
		focusRect = QRect(childWidget->mapTo(widget(), rect.topLeft().toPoint()), rect.size().toSize());
		focusRect.adjust(0, 0, 0, 100);
	}

	QScrollBar* scrollBar = verticalScrollBar();
	int value = focusRect.center().y() - visibleRect.center().y();

	if (scrollBar && value != 0)
	{
		QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
		anim->setDuration(300);
		anim->setStartValue(scrollBar->value());
		anim->setEndValue(scrollBar->value() + value);
		anim->setEasingCurve(QEasingCurve::InOutQuad);
		anim->start();
	}
}
