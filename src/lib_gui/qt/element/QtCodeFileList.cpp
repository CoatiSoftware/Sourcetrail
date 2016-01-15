#include "qt/element/QtCodeFileList.h"

#include <QPropertyAnimation>
#include <QScrollBar>
#include <QVariant>
#include <QVBoxLayout>

#include "utility/file/FileSystem.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFileList::QtCodeFileList(QWidget* parent)
	: QScrollArea(parent)
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
	QtCodeFile* file = getFile(locationFile);

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
	QtCodeFile* file = getFile(locationFile);
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

bool QtCodeFileList::scrollToFirstActiveSnippet()
{
	updateFiles();

	QtCodeSnippet* snippet = nullptr;
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		snippet = file->findFirstActiveSnippet();
		if (snippet)
		{
			if (!snippet->isVisible())
			{
				file->clickedSnippetButton();
			}

			emit shouldScrollToSnippet(snippet);
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

void QtCodeFileList::scrollToSnippet(QtCodeSnippet* snippet)
{
	this->ensureWidgetVisibleAnimated(snippet, snippet->getFirstActiveLineRect());
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
		anim->setDuration(500);
		anim->setStartValue(scrollBar->value());
		anim->setEndValue(scrollBar->value() + value);
		anim->setEasingCurve(QEasingCurve::InOutQuad);
		anim->start();
	}
}
