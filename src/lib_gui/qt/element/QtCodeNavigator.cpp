#include "qt/element/QtCodeNavigator.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageScrollCode.h"
#include "utility/messaging/type/MessageShowReference.h"

#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeNavigator::QtCodeNavigator(QWidget* parent)
	: QWidget(parent)
	, m_value(0)
	, m_refIndex(0)
	, m_scrollToFile(nullptr)
	, m_scrollToLine(0)
	, m_scrollToLocationId(0)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	{
		QWidget* navigation = new QWidget();
		QHBoxLayout* navLayout = new QHBoxLayout();
		navLayout->setSpacing(3);
		navLayout->setContentsMargins(7, 7, 7, 7);

		QPushButton* listButton = new QPushButton("list");
		navLayout->addWidget(listButton);

		QPushButton* fileButton = new QPushButton("file");
		fileButton->setEnabled(false);
		navLayout->addWidget(fileButton);

		navLayout->addStretch();

		m_refLabel = new QLabel("0/0 references");
		navLayout->addWidget(m_refLabel);

		navLayout->addStretch();

		m_prevButton = new QPushButton("<");
		m_nextButton = new QPushButton(">");

		m_prevButton->setToolTip("previous reference");
		m_nextButton->setToolTip("next reference");

		navLayout->addWidget(m_prevButton);
		navLayout->addWidget(m_nextButton);

		connect(m_prevButton, SIGNAL(clicked()), this, SLOT(previousReference()));
		connect(m_nextButton, SIGNAL(clicked()), this, SLOT(nextReference()));

		navigation->setLayout(navLayout);
		layout->addWidget(navigation);

		navigation->hide();
	}

	m_scrollArea = new QScrollArea(this);
	m_scrollArea->setObjectName("code_container");

	m_list = new QtCodeFileList(this);
	layout->addWidget(m_scrollArea);

	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setWidget(m_list);

	m_scrollSpeedChangeListener.setScrollBar(m_scrollArea->verticalScrollBar());

	connect(m_scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(scrolled(int)));
	connect(this, SIGNAL(shouldScrollToSnippet(QtCodeSnippet*, uint)),
		this, SLOT(scrollToSnippet(QtCodeSnippet*, uint)), Qt::QueuedConnection);
}

QtCodeNavigator::~QtCodeNavigator()
{
}

void QtCodeNavigator::addCodeSnippet(const CodeSnippetParams& params, bool insert)
{
	m_list->addCodeSnippet(params, insert);
}

void QtCodeNavigator::addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount, TimePoint modificationTime)
{
	m_list->addFile(locationFile, refCount, modificationTime);

	if (locationFile->isWholeCopy)
	{
		Reference ref;
		ref.filePath = locationFile->getFilePath();
		ref.tokenId = 0;
		ref.locationId = 0;

		m_references.push_back(ref);
	}
	else
	{
		locationFile->forEachStartTokenLocation(
			[&](TokenLocation* location)
			{
				if (!location->isScopeTokenLocation())
				{
					Reference ref;
					ref.filePath = location->getFilePath();
					ref.tokenId = location->getTokenId();
					ref.locationId = location->getId();

					m_references.push_back(ref);
				}
			}
		);
	}
}

void QtCodeNavigator::clearCodeSnippets()
{
	m_list->clearCodeSnippets();
	m_scrollArea->verticalScrollBar()->setValue(0);

	m_currentActiveTokenIds.clear();
	m_activeTokenIds.clear();
	m_activeLocalSymbolIds.clear();
	m_focusedTokenIds.clear();
	m_errorInfos.clear();

	m_references.clear();
	m_refIndex = 0;
}

const std::vector<Id>& QtCodeNavigator::getCurrentActiveTokenIds() const
{
	return m_currentActiveTokenIds;
}

void QtCodeNavigator::setCurrentActiveTokenIds(const std::vector<Id>& currentActiveTokenIds)
{
	m_currentActiveTokenIds = currentActiveTokenIds;
	m_currentActiveLocationIds.clear();
}

const std::vector<Id>& QtCodeNavigator::getCurrentActiveLocationIds() const
{
	return m_currentActiveLocationIds;
}

void QtCodeNavigator::setCurrentActiveLocationIds(const std::vector<Id>& currentActiveLocationIds)
{
	m_currentActiveLocationIds = currentActiveLocationIds;
	m_currentActiveTokenIds.clear();
}

const std::vector<Id>& QtCodeNavigator::getActiveTokenIds() const
{
	return m_activeTokenIds;
}

void QtCodeNavigator::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	setCurrentActiveTokenIds(activeTokenIds);

	m_activeTokenIds = activeTokenIds;
	m_activeLocalSymbolIds.clear();
}

const std::vector<Id>& QtCodeNavigator::getActiveLocalSymbolIds() const
{
	return m_activeLocalSymbolIds;
}

void QtCodeNavigator::setActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds)
{
	m_activeLocalSymbolIds = activeLocalSymbolIds;
}

const std::vector<Id>& QtCodeNavigator::getFocusedTokenIds() const
{
	return m_focusedTokenIds;
}

void QtCodeNavigator::setFocusedTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_focusedTokenIds = focusedTokenIds;
}

std::string QtCodeNavigator::getErrorMessageForId(Id errorId) const
{
	std::map<Id, ErrorInfo>::const_iterator it = m_errorInfos.find(errorId);

	if (it != m_errorInfos.end())
	{
		return it->second.message;
	}

	return "";
}

void QtCodeNavigator::setErrorInfos(const std::vector<ErrorInfo>& errorInfos)
{
	m_errorInfos.clear();

	for (const ErrorInfo& info : errorInfos)
	{
		m_errorInfos.emplace(info.id, info);
	}
}

bool QtCodeNavigator::hasErrors() const
{
	return m_errorInfos.size() > 0;
}

size_t QtCodeNavigator::getFatalErrorCountForFile(const FilePath& filePath) const
{
	size_t fatalErrorCount = 0;
	for (const std::pair<Id, ErrorInfo>& p : m_errorInfos)
	{
		const ErrorInfo& error = p.second;
		if (error.filePath == filePath && error.isFatal)
		{
			fatalErrorCount++;
		}
	}
	return fatalErrorCount;
}

void QtCodeNavigator::showActiveSnippet(
	const std::vector<Id>& activeTokenIds, std::shared_ptr<TokenLocationCollection> collection, bool scrollTo)
{
	if (activeTokenIds.size() != 1)
	{
		LOG_ERROR("Number of requested token ids to show is not 1.");
		return;
	}

	Id tokenId = activeTokenIds[0];

	std::vector<Id> locationIds;

	Id firstLocationId = 0;
	FilePath firstFilePath;
	std::set<FilePath> filePathsToExpand;

	std::map<FilePath, size_t> filePathOrder;

	for (size_t i = 0; i < m_references.size(); i++)
	{
		const Reference& ref = m_references[i];
		if (ref.tokenId == tokenId)
		{
			locationIds.push_back(ref.locationId);

			if (!firstLocationId)
			{
				firstLocationId = ref.locationId;
				firstFilePath = ref.filePath;
			}
		}

		filePathOrder.emplace(ref.filePath, filePathOrder.size());
	}

	if (!locationIds.size())
	{
		collection->forEachTokenLocation(
			[&](TokenLocation* location)
			{
				if (location->getTokenId() != tokenId)
				{
					return;
				}

				locationIds.push_back(location->getId());
				filePathsToExpand.insert(location->getFilePath());

				if (firstFilePath.empty() || filePathOrder[location->getFilePath()] < filePathOrder[firstFilePath])
				{
					firstFilePath = location->getFilePath();
					firstLocationId = location->getId();
				}
			}
		);
	}

	setCurrentActiveLocationIds(locationIds);
	updateFiles();

	for (const FilePath& filePath : filePathsToExpand)
	{
		m_list->requestFileContent(filePath);
	}

	if (firstLocationId)
	{
		showLocation(firstFilePath, firstLocationId, scrollTo);

		m_refIndex = 0;
		updateRefLabel();
	}
}

void QtCodeNavigator::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	setFocusedTokenIds(focusedTokenIds);
	updateFiles();
}

void QtCodeNavigator::defocusTokenIds()
{
	setFocusedTokenIds(std::vector<Id>());
	updateFiles();
}

void QtCodeNavigator::setFileMinimized(const FilePath path)
{
	m_list->setFileMinimized(path);
}

void QtCodeNavigator::setFileSnippets(const FilePath path)
{
	m_list->setFileSnippets(path);
}

void QtCodeNavigator::setFileMaximized(const FilePath path)
{
	m_list->setFileMaximized(path);
}

void QtCodeNavigator::setupFiles()
{
	std::set<FilePath> filePathsToExpand;
	for (const Reference& ref : m_references)
	{
		if (filePathsToExpand.find(ref.filePath) == filePathsToExpand.end())
		{
			m_list->requestFileContent(ref.filePath);
			filePathsToExpand.insert(ref.filePath);

			if (filePathsToExpand.size() >= 3)
			{
				break;
			}
		}
	}

	m_refIndex = 0;
	updateRefLabel();
}

void QtCodeNavigator::updateFiles()
{
	m_list->updateFiles();
}

void QtCodeNavigator::showContents()
{
	m_list->showContents();
}

void QtCodeNavigator::showLocation(const FilePath& filePath, Id locationId, bool scrollTo)
{
	updateFiles();

	QtCodeFile* file = m_list->getFile(filePath);

	if (file->isCollapsed())
	{
		file->requestContent();

		if (scrollTo)
		{
			m_scrollToFile = file;
			m_scrollToLocationId = locationId;
		}
	}
	else
	{
		scrollToLocation(file, locationId, scrollTo);
	}
}

void QtCodeNavigator::scrollToValue(int value)
{
	m_value = value;
	QTimer::singleShot(100, this, SLOT(setValue()));
}

void QtCodeNavigator::scrollToLine(const FilePath& filePath, unsigned int line)
{
	QtCodeFile* file = m_list->getFile(filePath);

	if (!file)
	{
		return;
	}

	if (file->isCollapsed())
	{
		file->requestContent();

		m_scrollToFile = file;
		m_scrollToLine = line;
	}
	else if (file->getFileSnippet())
	{
		emit shouldScrollToSnippet(file->getFileSnippet(), line);
	}
	else
	{
		m_scrollToFile = file;
		m_scrollToLine = line;
		scrollToSnippetIfRequested();
	}
}

void QtCodeNavigator::scrollToLocation(QtCodeFile* file, Id locationId, bool scrollTo)
{
	QtCodeSnippet* snippet = nullptr;

	if (locationId)
	{
		snippet = file->getSnippetForLocationId(locationId);
	}
	else
	{
		snippet = file->getFileSnippet();
	}

	if (!snippet)
	{
		return;
	}

	if (!snippet->isVisible())
	{
		file->setSnippets();
	}

	if (scrollTo)
	{
		if (locationId)
		{
			emit shouldScrollToSnippet(snippet, snippet->getLineNumberForLocationId(locationId));
		}
		else
		{
			emit shouldScrollToSnippet(snippet, 1);
		}
	}
}

void QtCodeNavigator::scrollToSnippetIfRequested()
{
	if (m_scrollToFile && m_scrollToLine)
	{
		emit shouldScrollToSnippet(m_scrollToFile->getSnippetForLine(m_scrollToLine), m_scrollToLine);
	}
	else if (m_scrollToFile && m_scrollToFile->hasSnippets())
	{
		scrollToLocation(m_scrollToFile, m_scrollToLocationId, true);
	}

	m_scrollToFile = nullptr;
	m_scrollToLocationId = 0;
	m_scrollToLine = 0;
}

void QtCodeNavigator::requestScrollToLine(QtCodeFile* file, unsigned int line)
{
	m_scrollToFile = file;
	m_scrollToLine = line;
}

void QtCodeNavigator::scrolled(int value)
{
	MessageScrollCode(value).dispatch();
}

void QtCodeNavigator::scrollToSnippet(QtCodeSnippet* snippet, uint lineNumber)
{
	if (lineNumber)
	{
		this->ensureWidgetVisibleAnimated(snippet, snippet->getLineRectForLineNumber(lineNumber));
	}
}

void QtCodeNavigator::setValue()
{
	m_scrollArea->verticalScrollBar()->setValue(m_value);
}

void QtCodeNavigator::previousReference()
{
	if (!m_references.size())
	{
		return;
	}

	if (m_refIndex < 2)
	{
		m_refIndex = m_references.size();
	}
	else
	{
		m_refIndex--;
	}

	showCurrentReference();
}

void QtCodeNavigator::nextReference()
{
	if (!m_references.size())
	{
		return;
	}

	m_refIndex++;

	if (m_refIndex == m_references.size() + 1)
	{
		m_refIndex = 1;
	}

	showCurrentReference();
}

void QtCodeNavigator::showCurrentReference()
{
	const Reference& ref = m_references[m_refIndex - 1];

	setCurrentActiveLocationIds(std::vector<Id>(1, ref.locationId));
	showLocation(ref.filePath, ref.locationId, true);

	updateRefLabel();

	MessageShowReference(m_refIndex, ref.tokenId, ref.locationId).dispatch();
}

void QtCodeNavigator::updateRefLabel()
{
	size_t n = m_references.size();
	size_t t = m_refIndex;

	if (t)
	{
		m_refLabel->setText(QString::number(t) + "/" + QString::number(n) + " references");
	}
	else
	{
		m_refLabel->setText(QString::number(n) + " references");
	}

	m_prevButton->setEnabled(n > 1);
	m_nextButton->setEnabled(n > 1);
}

void QtCodeNavigator::ensureWidgetVisibleAnimated(QWidget *childWidget, QRectF rect)
{
	QScrollArea* area = m_scrollArea;

	if (!area->widget()->isAncestorOf(childWidget))
	{
		return;
	}

	const QRect microFocus = childWidget->inputMethodQuery(Qt::ImCursorRectangle).toRect();
	const QRect defaultMicroFocus = childWidget->QWidget::inputMethodQuery(Qt::ImCursorRectangle).toRect();
	QRect focusRect = (microFocus != defaultMicroFocus)
		? QRect(childWidget->mapTo(area->widget(), microFocus.topLeft()), microFocus.size())
		: QRect(childWidget->mapTo(area->widget(), QPoint(0, 0)), childWidget->size());
	const QRect visibleRect(-area->widget()->pos(), area->viewport()->size());

	if (rect.height() > 0)
	{
		focusRect = QRect(childWidget->mapTo(area->widget(), rect.topLeft().toPoint()), rect.size().toSize());
		focusRect.adjust(0, 0, 0, 100);
	}

	QScrollBar* scrollBar = area->verticalScrollBar();
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

void QtCodeNavigator::handleMessage(MessageCodeReference* message)
{
	MessageCodeReference::ReferenceType type = message->type;

	m_onQtThread(
		[=]()
		{
			if (type == MessageCodeReference::REFERENCE_PREVIOUS)
			{
				previousReference();
			}
			else if (type == MessageCodeReference::REFERENCE_NEXT)
			{
				nextReference();
			}
		}
	);
}

void QtCodeNavigator::handleMessage(MessageWindowFocus* message)
{
	m_onQtThread(
		[=]()
		{
			m_list->onWindowFocus();
		}
	);
}
