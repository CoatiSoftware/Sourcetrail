#include "qt/element/QtCodeNavigator.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageScrollCode.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/ResourcePaths.h"

#include "data/location/SourceLocation.h"
#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"
#include "qt/element/QtCodeArea.h"
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeSnippet.h"
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"

QtCodeNavigator::QtCodeNavigator(QWidget* parent)
	: QWidget(parent)
	, m_mode(MODE_NONE)
	, m_oldMode(MODE_NONE)
	, m_activeTokenId(0)
	, m_value(0)
	, m_refIndex(0)
	, m_singleHasNewFile(false)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	{
		QWidget* navigation = new QWidget();
		navigation->setObjectName("code_navigation");

		QHBoxLayout* navLayout = new QHBoxLayout();
		navLayout->setSpacing(2);
		navLayout->setContentsMargins(7, 7, 7, 6);


		m_prevButton = new QPushButton();
		m_nextButton = new QPushButton();

		m_prevButton->setObjectName("reference_button_previous");
		m_nextButton->setObjectName("reference_button_next");

		m_prevButton->setToolTip("previous reference");
		m_nextButton->setToolTip("next reference");

		navLayout->addWidget(m_prevButton);
		navLayout->addWidget(m_nextButton);

		connect(m_prevButton, &QPushButton::clicked, this, &QtCodeNavigator::previousReference);
		connect(m_nextButton, &QPushButton::clicked, this, &QtCodeNavigator::nextReference);


		m_refLabel = new QLabel("0/0 references");
		m_refLabel->setObjectName("references_label");
		navLayout->addWidget(m_refLabel);

		navLayout->addStretch();


		m_listButton = new QPushButton();
		m_fileButton = new QPushButton();

		m_listButton->setObjectName("mode_button_list");
		m_fileButton->setObjectName("mode_button_single");

		m_listButton->setToolTip("snippet list mode");
		m_fileButton->setToolTip("single file mode");

		m_listButton->setCheckable(true);
		m_fileButton->setCheckable(true);

		navLayout->addWidget(m_listButton);
		navLayout->addWidget(m_fileButton);

		connect(m_listButton, &QPushButton::clicked, this, &QtCodeNavigator::setModeList);
		connect(m_fileButton, &QPushButton::clicked, this, &QtCodeNavigator::setModeSingle);


		navigation->setLayout(navLayout);
		layout->addWidget(navigation);

		m_separatorLine = new QFrame();
		m_separatorLine->setFrameShape(QFrame::HLine);
		m_separatorLine->setFrameShadow(QFrame::Plain);
		m_separatorLine->setObjectName("separator_line");
		m_separatorLine->setFixedHeight(1);
		m_separatorLine->hide();
		layout->addWidget(m_separatorLine);
	}

	m_list = new QtCodeFileList(this);
	layout->addWidget(m_list);

	m_single = new QtCodeFileSingle(this);
	layout->addWidget(m_single);

	setMode(ApplicationSettings::getInstance()->getCodeViewModeSingle() ? MODE_SINGLE : MODE_LIST);
	showContents();

	refreshStyle();

	connect(this, &QtCodeNavigator::scrollRequest, this, &QtCodeNavigator::handleScrollRequest, Qt::QueuedConnection);
}

QtCodeNavigator::~QtCodeNavigator()
{
}

void QtCodeNavigator::addCodeSnippet(const CodeSnippetParams& params)
{
	FilePath currentPath = m_single->getCurrentFilePath();

	if (params.reduced)
	{
		m_list->addCodeSnippet(params);
		m_single->addCodeSnippet(params);
	}
	else
	{
		m_current->addCodeSnippet(params);
	}

	if (currentPath != m_single->getCurrentFilePath())
	{
		m_singleHasNewFile = true;
	}
}

void QtCodeNavigator::addFile(std::shared_ptr<SourceLocationFile> locationFile, int refCount, TimeStamp modificationTime)
{
	bool firstFile = m_references.size() == 0;

	m_list->addFile(locationFile->getFilePath(), locationFile->isWhole(), refCount, modificationTime, locationFile->isComplete());

	if (locationFile->isWhole())
	{
		Reference ref;
		ref.filePath = locationFile->getFilePath();
		m_references.push_back(ref);
	}
	else
	{
		locationFile->forEachStartSourceLocation(
			[&](SourceLocation* location)
			{
				if (location->isScopeLocation())
				{
					return;
				}

				if (!location->getTokenIds().size())
				{
					Reference ref;
					ref.filePath = location->getFilePath();
					ref.tokenId = 0;
					ref.locationId = location->getLocationId();
					ref.locationType = location->getType();

					m_references.push_back(ref);
					return;
				}

				for (Id i : location->getTokenIds())
				{
					Reference ref;
					ref.filePath = location->getFilePath();
					ref.tokenId = i;
					ref.locationId = location->getLocationId();
					ref.locationType = location->getType();

					m_references.push_back(ref);
				}
			}
		);
	}

	if (firstFile && m_references.size() && m_references[0].locationType != LOCATION_TOKEN)
	{
		clearCaches();
	}
}

void QtCodeNavigator::addedFiles()
{
	if (m_mode == MODE_SINGLE && m_references.size())
	{
		if (!m_refIndex || !m_activeReference.tokenId)
		{
			m_single->requestFileContent(m_references.front().filePath);
		}
	}

	if (m_refIndex == 0)
	{
		updateRefLabel();
	}
}

void QtCodeNavigator::clear()
{
	clearCodeSnippets();
	clearCaches();

	updateRefLabel();
}

void QtCodeNavigator::clearCodeSnippets()
{
	m_list->clear();

	m_currentActiveTokenIds.clear();
	m_activeTokenIds.clear();
	m_activeLocalSymbolIds.clear();
	m_focusedTokenIds.clear();
	m_errorInfos.clear();

	m_activeTokenId = 0;

	if (m_references.size() && m_references[0].locationType != LOCATION_TOKEN)
	{
		clearCaches();
	}

	m_references.clear();
	m_activeReference = Reference();
	m_refIndex = 0;

	m_singleHasNewFile = false;

	clearScreenMatches();
}

void QtCodeNavigator::clearFile()
{
	m_single->clearFile();

	updateRefLabel();
}

void QtCodeNavigator::clearCaches()
{
	m_single->clearCache();
}

void QtCodeNavigator::clearSnippetReferences()
{
	m_list->clearSnippetTitleAndScrollBar();
}

void QtCodeNavigator::setMode(Mode mode)
{
	m_mode = mode;

	if (mode == MODE_LIST)
	{
		m_current = m_list;
	}
	else
	{
		m_current = m_single;
	}
}

const std::set<Id>& QtCodeNavigator::getCurrentActiveTokenIds() const
{
	return m_currentActiveTokenIds;
}

void QtCodeNavigator::setCurrentActiveTokenIds(const std::vector<Id>& currentActiveTokenIds)
{
	m_currentActiveTokenIds = std::set<Id>(currentActiveTokenIds.begin(), currentActiveTokenIds.end());
	m_currentActiveLocationIds.clear();
}

const std::set<Id>& QtCodeNavigator::getCurrentActiveLocationIds() const
{
	return m_currentActiveLocationIds;
}

void QtCodeNavigator::setCurrentActiveLocationIds(const std::vector<Id>& currentActiveLocationIds)
{
	m_currentActiveLocationIds = std::set<Id>(currentActiveLocationIds.begin(), currentActiveLocationIds.end());
	m_currentActiveTokenIds.clear();
}

const std::set<Id>& QtCodeNavigator::getActiveTokenIds() const
{
	return m_activeTokenIds;
}

void QtCodeNavigator::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	setCurrentActiveTokenIds(activeTokenIds);

	m_activeTokenIds = std::set<Id>(activeTokenIds.begin(), activeTokenIds.end());
	m_activeTokenId = activeTokenIds.size() ? activeTokenIds[0] : 0;

	m_activeLocalSymbolIds.clear();
}

const std::set<Id>& QtCodeNavigator::getActiveLocalSymbolIds() const
{
	return m_activeLocalSymbolIds;
}

void QtCodeNavigator::setActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds)
{
	m_activeLocalSymbolIds = std::set<Id>(activeLocalSymbolIds.begin(), activeLocalSymbolIds.end());
}

const std::set<Id>& QtCodeNavigator::getFocusedTokenIds() const
{
	return m_focusedTokenIds;
}

void QtCodeNavigator::setFocusedTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_focusedTokenIds = std::set<Id>(focusedTokenIds.begin(), focusedTokenIds.end());
}

std::wstring QtCodeNavigator::getErrorMessageForId(Id errorId) const
{
	std::map<Id, ErrorInfo>::const_iterator it = m_errorInfos.find(errorId);

	if (it != m_errorInfos.end())
	{
		return it->second.message;
	}

	return L"";
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
		if (error.filePath == filePath.wstr() && error.fatal)
		{
			fatalErrorCount++;
		}
	}
	return fatalErrorCount;
}

bool QtCodeNavigator::isInListMode() const
{
	return m_mode == MODE_LIST;
}

bool QtCodeNavigator::hasSingleFileCached(const FilePath& filePath) const
{
	return m_single->hasFileCached(filePath);
}

void QtCodeNavigator::showActiveSnippet(
	const std::vector<Id>& activeTokenIds, std::shared_ptr<SourceLocationCollection> collection, bool scrollTo)
{
	if (activeTokenIds.size() != 1)
	{
		LOG_ERROR("Number of requested token ids to show is not 1.");
		return;
	}

	m_activeReference = Reference();
	Id tokenId = activeTokenIds[0];

	std::vector<Id> locationIds;
	size_t refIndex = 0;
	Reference firstReference;

	std::map<FilePath, size_t> filePathOrder;

	for (size_t i = 0; i < m_references.size(); i++)
	{
		const Reference& ref = m_references[i];
		if (ref.tokenId == tokenId)
		{
			locationIds.push_back(ref.locationId);

			if (!firstReference.tokenId)
			{
				firstReference = ref;
				refIndex = i + 1;
			}
		}

		filePathOrder.emplace(ref.filePath, filePathOrder.size());
	}

	std::set<FilePath> filePathsToExpand;
	if (!locationIds.size())
	{
		collection->forEachSourceLocation(
			[&](SourceLocation* location)
			{
				bool foundId = false;
				for (Id i : location->getTokenIds())
				{
					if (i == tokenId)
					{
						foundId = true;
						break;
					}
				}

				if (!foundId)
				{
					return;
				}

				locationIds.push_back(location->getLocationId());
				filePathsToExpand.insert(location->getFilePath());

				if (!firstReference.tokenId || filePathOrder[location->getFilePath()] < filePathOrder[firstReference.filePath])
				{
					firstReference.tokenId = tokenId;
					firstReference.locationId = location->getLocationId();
					firstReference.filePath = location->getFilePath();
				}
			}
		);
	}

	setCurrentActiveLocationIds(locationIds);
	updateFiles();

	if (m_mode == MODE_LIST)
	{
		for (const FilePath& filePath : filePathsToExpand)
		{
			m_list->requestFileContent(filePath);
		}
	}
	else if (firstReference.tokenId)
	{
		m_single->requestFileContent(firstReference.filePath);
	}

	if (firstReference.tokenId)
	{
		if (scrollTo)
		{
			requestScroll(firstReference.filePath, 0, firstReference.locationId, true, QtCodeNavigateable::SCROLL_CENTER);
			emit scrollRequest();
		}

		m_refIndex = refIndex;
		updateRefLabel();

		if (!refIndex)
		{
			m_activeReference = firstReference;
		}
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
	if (m_mode == MODE_LIST)
	{
		m_list->setFileMinimized(path);
	}
}

void QtCodeNavigator::setFileSnippets(const FilePath path)
{
	if (m_mode == MODE_LIST)
	{
		m_list->setFileSnippets(path);
	}
}

void QtCodeNavigator::setFileMaximized(const FilePath path)
{
	if (m_mode == MODE_LIST)
	{
		m_list->setFileMaximized(path);
	}
}

void QtCodeNavigator::updateFiles()
{
	m_current->updateFiles();
	updateRefLabel();
}

void QtCodeNavigator::showContents()
{
	if (m_oldMode != m_mode)
	{
		m_listButton->setChecked(m_mode == MODE_LIST);
		m_fileButton->setChecked(m_mode == MODE_SINGLE);

		switch (m_mode)
		{
			case MODE_SINGLE:
				m_list->hide();
				m_single->show();
				m_separatorLine->hide();
				break;

			case MODE_LIST:
				m_single->hide();
				m_list->show();
				m_separatorLine->show();
				break;

			default:
				LOG_ERROR("Wrong mode set in code navigator");
				return;
		}

		ApplicationSettings::getInstance()->setCodeViewModeSingle(m_mode == MODE_SINGLE);
		ApplicationSettings::getInstance()->save();

		m_oldMode = m_mode;
	}

	m_current->showContents();
}

void QtCodeNavigator::refreshStyle()
{
	float height = std::max(ApplicationSettings::getInstance()->getFontSize() + 10, 24);

	m_prevButton->setFixedHeight(height);
	m_nextButton->setFixedHeight(height);
	m_listButton->setFixedHeight(height);
	m_fileButton->setFixedHeight(height);

	m_prevButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_left.png"),
		"search/button"
	));

	m_nextButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_right.png"),
		"search/button"
	));

	m_listButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().concatenate(L"code_view/images/list.png"),
		"search/button"
	));

	m_fileButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().concatenate(L"code_view/images/file.png"),
		"search/button"
	));

	m_prevButton->setIconSize(QSize(12, 12));
	m_nextButton->setIconSize(QSize(12, 12));
	m_listButton->setIconSize(QSize(14, 14));
	m_fileButton->setIconSize(QSize(14, 14));

	clearCaches();
}

size_t QtCodeNavigator::findScreenMatches(const std::string& query)
{
	clearScreenMatches();

	m_current->findScreenMatches(query, &m_screenMatches);

	return m_screenMatches.size();
}

void QtCodeNavigator::activateScreenMatch(size_t matchIndex)
{
	if (matchIndex >= m_screenMatches.size())
	{
		return;
	}

	std::pair<QtCodeArea*, Id> p = m_screenMatches[matchIndex];
	m_activeScreenMatchId = p.second;
	m_currentActiveLocationIds.insert(m_activeScreenMatchId);
	p.first->updateContent();

	requestScroll(
		p.first->getSourceLocationFile()->getFilePath(), 0, m_activeScreenMatchId, true, QtCodeNavigateable::SCROLL_CENTER);
	emit scrollRequest();
}

void QtCodeNavigator::deactivateScreenMatch(size_t matchIndex)
{
	if (matchIndex >= m_screenMatches.size())
	{
		return;
	}

	m_currentActiveLocationIds.erase(m_screenMatches[matchIndex].second);
	m_screenMatches[matchIndex].first->updateContent();

	m_activeScreenMatchId = 0;
}

void QtCodeNavigator::clearScreenMatches()
{
	if (m_activeScreenMatchId)
	{
		m_currentActiveLocationIds.erase(m_activeScreenMatchId);
		m_activeScreenMatchId = 0;
	}

	for (auto p : m_screenMatches)
	{
		p.first->clearScreenMatches();
	}

	m_screenMatches.clear();
}

void QtCodeNavigator::scrollToValue(int value, bool inListMode)
{
	if ((m_mode == MODE_LIST) == inListMode)
	{
		m_value = value;
		QTimer::singleShot(100, this, &QtCodeNavigator::setValue);
		m_scrollRequest = ScrollRequest();
	}
}

void QtCodeNavigator::scrollToLine(const FilePath& filePath, unsigned int line)
{
	requestScroll(filePath, line, 0, false, QtCodeNavigateable::SCROLL_CENTER);
	emit scrollRequest();
}

void QtCodeNavigator::scrollToDefinition(bool animated, bool ignoreActiveReference)
{
	if (ignoreActiveReference)
	{
		m_activeReference = Reference();
		m_refIndex = 0;
		updateRefLabel();
	}

	if (m_activeReference.tokenId)
	{
		if (m_mode == MODE_LIST)
		{
			m_list->requestFileContent(m_activeReference.filePath);
		}

		requestScroll(m_activeReference.filePath, 0, m_activeReference.locationId, true, QtCodeNavigateable::SCROLL_CENTER);
		emit scrollRequest();

		updateRefLabel();
		return;
	}

	if (m_refIndex != 0)
	{
		showCurrentReference(false);
		return;
	}

	if (!m_activeTokenId)
	{
		if (m_references.size() && m_references.front().locationType != LOCATION_TOKEN)
		{
			requestScroll(
				m_references.front().filePath, 0, m_references.front().locationId, false, QtCodeNavigateable::SCROLL_CENTER);
			emit scrollRequest();
		}
		return;
	}

	if (m_mode == MODE_LIST)
	{
		std::pair<QtCodeSnippet*, Id> result = m_list->getFirstSnippetWithActiveLocationId(m_activeTokenId);
		if (result.first != nullptr)
		{
			requestScroll(
				result.first->getFile()->getFilePath(), 0, result.second, animated, QtCodeNavigateable::SCROLL_CENTER);
			emit scrollRequest();
			return;
		}
	}
	else
	{
		Id locationId = m_single->getLocationIdOfFirstActiveLocationOfTokenId(m_activeTokenId);
		if (locationId)
		{
			requestScroll(m_single->getCurrentFilePath(), 0, locationId, true, QtCodeNavigateable::SCROLL_CENTER);
			emit scrollRequest();
			return;
		}
	}

	if (m_references.size())
	{
		m_current->requestFileContent(m_references.front().filePath);
		requestScroll(m_references.front().filePath, 0, m_references.front().locationId, false, QtCodeNavigateable::SCROLL_CENTER);
		emit scrollRequest();
	}
}

void QtCodeNavigator::scrollToSnippetIfRequested()
{
	emit scrollRequest();
}

void QtCodeNavigator::requestScroll(
	const FilePath& filePath, uint lineNumber, Id locationId, bool animated, QtCodeNavigateable::ScrollTarget target)
{
	ScrollRequest req;
	req.filePath = filePath;
	req.lineNumber = lineNumber;
	req.locationId = locationId;
	req.animated = animated;
	req.target = target;

	if (m_mode == MODE_SINGLE)
	{
		if (req.lineNumber || m_singleHasNewFile)
		{
			req.animated = false;
		}
	}

	// std::cout << "scroll request: " << req.filePath.str() << " " << req.lineNumber << " " << req.locationId;
	// std::cout << " " << req.animated << " " << req.target << std::endl;

	if ((!m_scrollRequest.lineNumber && !m_scrollRequest.locationId) && !req.filePath.empty())
	{
		m_scrollRequest = req;
	}

	m_singleHasNewFile = false;
}

void QtCodeNavigator::handleScrollRequest()
{
	const ScrollRequest& req = m_scrollRequest;
	if (req.filePath.empty())
	{
		return;
	}

	// std::cout << "handle scroll request: " << req.filePath.str() << " " << req.lineNumber << " " << req.locationId;
	// std::cout << " " << req.animated << " " << req.target << std::endl;

	bool done = m_current->requestScroll(req.filePath, req.lineNumber, req.locationId, req.animated, req.target);
	if (done)
	{
		m_scrollRequest = ScrollRequest();
	}
	else if (m_mode == MODE_SINGLE)
	{
		m_scrollRequest.animated = false;
	}
}

void QtCodeNavigator::scrolled(int value)
{
	MessageScrollCode(value, m_mode == MODE_LIST).dispatch();
}

void QtCodeNavigator::setValue()
{
	QAbstractScrollArea* area = m_current->getScrollArea();

	if (area)
	{
		area->verticalScrollBar()->setValue(m_value);
	}
}

void QtCodeNavigator::previousReference(bool fromUI)
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

	m_activeReference = Reference();

	showCurrentReference(fromUI);
}

void QtCodeNavigator::nextReference(bool fromUI)
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

	m_activeReference = Reference();

	showCurrentReference(fromUI);
}

void QtCodeNavigator::setModeList()
{
	m_single->clickedSnippetButton();
}

void QtCodeNavigator::setModeSingle()
{
	m_list->maximizeFirstFile();
}

void QtCodeNavigator::showCurrentReference(bool fromUI)
{
	const Reference& ref = m_references[m_refIndex - 1];
	MessageShowReference(m_refIndex, ref.tokenId, ref.locationId, fromUI).dispatch();
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

void QtCodeNavigator::handleMessage(MessageFinishedParsing* message)
{
	m_onQtThread(
		[=]()
		{
			clearCaches();
		}
	);
}

void QtCodeNavigator::handleMessage(MessageShowReference* message)
{
	size_t refIndex = message->refIndex;
	bool replayed = message->isReplayed();

	m_onQtThread(
		[=]()
		{
			m_refIndex = refIndex;

			if (m_refIndex > 0)
			{
				const Reference& ref = m_references[m_refIndex - 1];
				setCurrentActiveLocationIds(std::vector<Id>(1, ref.locationId));

				if (!replayed)
				{
					updateFiles();
				}

				requestScroll(ref.filePath, 0, ref.locationId, true, QtCodeNavigateable::SCROLL_CENTER);
				emit scrollRequest();

				if (ref.locationType == LOCATION_ERROR)
				{
					MessageShowErrors(ref.tokenId).dispatch();
				}
			}

			if (!replayed)
			{
				updateRefLabel();
			}
		}
	);
}

void QtCodeNavigator::handleMessage(MessageSwitchColorScheme* message)
{
	m_onQtThread(
		[=]()
		{
			clearCaches();
		}
	);
}

void QtCodeNavigator::handleMessage(MessageWindowFocus* message)
{
	if (message->focusIn)
	{
		m_onQtThread(
			[=]()
			{
				m_current->onWindowFocus();
			}
		);
	}
}
