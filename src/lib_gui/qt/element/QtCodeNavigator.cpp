#include "QtCodeNavigator.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "logging.h"
#include "MessageShowError.h"
#include "MessageScrollCode.h"
#include "ResourcePaths.h"
#include "utility.h"

#include "SourceLocation.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"
#include "QtCodeArea.h"
#include "QtCodeFile.h"
#include "QtCodeSnippet.h"
#include "QtSearchBarButton.h"
#include "utilityQt.h"
#include "ApplicationSettings.h"

QtCodeNavigator::QtCodeNavigator(QWidget* parent)
	: QWidget(parent)
	, m_mode(MODE_NONE)
	, m_oldMode(MODE_NONE)
	, m_activeTokenId(0)
	, m_value(0)
	, m_refIndex(0)
	, m_singleHasNewFile(false)
	, m_useSingleFileCache(false)
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

		{
			m_prevFileButton =
				new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_left.png"), true);
			m_nextFileButton =
				new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_right.png"), true);
			m_prevReferenceButton =
				new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_up.png"), true);
			m_nextReferenceButton =
				new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_down.png"), true);

			m_prevFileButton->setObjectName("file_button_previous");
			m_nextFileButton->setObjectName("file_button_next");
			m_prevReferenceButton->setObjectName("reference_button_previous");
			m_nextReferenceButton->setObjectName("reference_button_next");

			m_prevFileButton->setToolTip("previous file");
			m_nextFileButton->setToolTip("next file");
			m_prevReferenceButton->setToolTip("previous reference");
			m_nextReferenceButton->setToolTip("next reference");

			m_prevFileButton->setIconSize(QSize(12, 12));
			m_nextFileButton->setIconSize(QSize(12, 12));
			m_prevReferenceButton->setIconSize(QSize(12, 12));
			m_nextReferenceButton->setIconSize(QSize(12, 12));

			m_prevFileButton->hide();
			m_nextFileButton->hide();

			navLayout->addWidget(m_prevFileButton);
			navLayout->addWidget(m_prevReferenceButton);
			navLayout->addWidget(m_nextReferenceButton);
			navLayout->addWidget(m_nextFileButton);

			connect(m_prevFileButton, &QPushButton::clicked, this, &QtCodeNavigator::previousFile);
			connect(m_nextFileButton, &QPushButton::clicked, this, &QtCodeNavigator::nextFile);
			connect(m_prevReferenceButton, &QPushButton::clicked, this, &QtCodeNavigator::previousReference);
			connect(m_nextReferenceButton, &QPushButton::clicked, this, &QtCodeNavigator::nextReference);

			// m_refLabel = new QLabel("0 files  |  0 references");
			m_refLabel = new QLabel("0 references");
			m_refLabel->setObjectName("references_label");
			navLayout->addWidget(m_refLabel);

			navLayout->addStretch();
		}

		{
			m_prevLocalReferenceButton =
				new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_up.png"), true);
			m_nextLocalReferenceButton =
				new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_down.png"), true);

			m_prevLocalReferenceButton->setObjectName("local_reference_button_previous");
			m_nextLocalReferenceButton->setObjectName("local_reference_button_next");

			m_prevLocalReferenceButton->setToolTip("previous local reference");
			m_nextLocalReferenceButton->setToolTip("next local reference");

			m_prevLocalReferenceButton->setIconSize(QSize(12, 12));
			m_nextLocalReferenceButton->setIconSize(QSize(12, 12));

			navLayout->addWidget(m_prevLocalReferenceButton);
			navLayout->addWidget(m_nextLocalReferenceButton);

			connect(m_prevLocalReferenceButton, &QPushButton::clicked, this, &QtCodeNavigator::previousLocalReference);
			connect(m_nextLocalReferenceButton, &QPushButton::clicked, this, &QtCodeNavigator::nextLocalReference);

			m_localRefLabel = new QLabel("0/0 local references");
			m_localRefLabel->setObjectName("references_label");
			navLayout->addWidget(m_localRefLabel);

			navLayout->addStretch();

			utility::setWidgetRetainsSpaceWhenHidden(m_prevLocalReferenceButton);
			utility::setWidgetRetainsSpaceWhenHidden(m_nextLocalReferenceButton);
			utility::setWidgetRetainsSpaceWhenHidden(m_localRefLabel);

			m_prevLocalReferenceButton->hide();
			m_nextLocalReferenceButton->hide();
			m_localRefLabel->hide();
		}

		m_listButton = new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/list.png"), true);
		m_fileButton = new QtSearchBarButton(ResourcePaths::getGuiPath().concatenate(L"code_view/images/file.png"), true);

		m_listButton->setObjectName("mode_button_list");
		m_fileButton->setObjectName("mode_button_single");

		m_listButton->setToolTip("snippet list mode");
		m_fileButton->setToolTip("single file mode");

		m_listButton->setCheckable(true);
		m_fileButton->setCheckable(true);

		m_listButton->setIconSize(QSize(14, 14));
		m_fileButton->setIconSize(QSize(14, 14));

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

	// refresh local reference count when visible code changes
	if (m_localReferences.size() && m_localReferences[0].locationType == LOCATION_LOCAL_SYMBOL)
	{
		setActiveLocalTokenIds(utility::toVector(m_activeLocalTokenIds), LOCATION_LOCAL_SYMBOL);
	}
}

void QtCodeNavigator::updateCodeSnippet(const CodeSnippetParams& params)
{
	m_current->updateCodeSnippet(params);
}

void QtCodeNavigator::addFile(std::shared_ptr<SourceLocationFile> locationFile, int refCount, TimeStamp modificationTime)
{
	m_list->addFile(locationFile, refCount, modificationTime);

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
				if (location->isScopeLocation() || location->getType() == LOCATION_SIGNATURE || location->getType() == LOCATION_COMMENT)
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
		updateRefLabels();
	}
}

void QtCodeNavigator::clear()
{
	clearCodeSnippets(false);
	clearCaches();

	updateRefLabels();
}

void QtCodeNavigator::clearCodeSnippets(bool useSingleFileCache)
{
	m_list->clear();

	m_currentActiveTokenIds.clear();
	m_activeTokenIds.clear();
	m_activeLocalTokenIds.clear();
	m_focusedTokenIds.clear();
	m_errorInfos.clear();

	m_activeTokenId = 0;

	if (!m_useSingleFileCache || !useSingleFileCache)
	{
		clearCaches();
	}
	m_useSingleFileCache = useSingleFileCache;

	m_references.clear();
	m_activeReference = Reference();
	m_refIndex = 0;

	m_singleHasNewFile = false;

	clearScreenMatches();
}

void QtCodeNavigator::clearFile()
{
	m_single->clearFile();

	updateRefLabels();
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
	setActiveLocalTokenIds({ }, LOCATION_TOKEN);

	m_currentActiveLocationIds = std::set<Id>(currentActiveLocationIds.begin(), currentActiveLocationIds.end());
	m_currentActiveTokenIds.clear();
}

const std::set<Id>& QtCodeNavigator::getCurrentActiveLocalLocationIds() const
{
	return m_currentActiveLocalLocationIds;
}

void QtCodeNavigator::setCurrentActiveLocalLocationIds(const std::vector<Id>& currentActiveLocalLocationIds)
{
	m_currentActiveLocalLocationIds = std::set<Id>(currentActiveLocalLocationIds.begin(), currentActiveLocalLocationIds.end());
}

const std::set<Id>& QtCodeNavigator::getActiveTokenIds() const
{
	return m_activeTokenIds;
}

void QtCodeNavigator::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	setActiveLocalTokenIds({ }, LOCATION_TOKEN);
	setCurrentActiveTokenIds(activeTokenIds);

	m_activeTokenIds = std::set<Id>(activeTokenIds.begin(), activeTokenIds.end());
	m_activeTokenId = activeTokenIds.size() ? activeTokenIds[0] : 0;
}

const std::set<Id>& QtCodeNavigator::getActiveLocalTokenIds() const
{
	return m_activeLocalTokenIds;
}

void QtCodeNavigator::setActiveLocalTokenIds(const std::vector<Id>& activeLocalTokenIds, LocationType locationType)
{
	setCurrentActiveTokenIds(locationType == LOCATION_TOKEN ? activeLocalTokenIds : std::vector<Id>());
	setCurrentActiveLocalLocationIds({ });

	m_activeLocalTokenIds.clear();
	m_activeLocalTokenIds.insert(activeLocalTokenIds.begin(), activeLocalTokenIds.end());

	m_localReferences.clear();
	m_localRefIndex = 0;

	if (m_activeLocalTokenIds.size())
	{
		for (std::pair<FilePath, Id> p : m_current->getLocationIdsForTokenIds(m_activeLocalTokenIds))
		{
			Reference ref;
			ref.filePath = p.first;
			ref.locationId = p.second;
			ref.locationType = locationType;
			m_localReferences.push_back(ref);
		}
	}
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
	return m_useSingleFileCache && m_single->hasFileCached(filePath);
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

	// iterate local references when same tokenId get reactivated (consecutive edge clicks)
	if (m_activeLocalTokenIds.size() == 1 && *m_activeLocalTokenIds.begin() == tokenId && m_localReferences.size())
	{
		nextLocalReference();
		return;
	}

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

	setActiveLocalTokenIds({ tokenId }, LOCATION_TOKEN);
	updateFiles();

	if (firstReference.tokenId)
	{
		if (scrollTo)
		{
			requestScroll(firstReference.filePath, 0, firstReference.locationId, true, QtCodeNavigateable::SCROLL_CENTER);
			emit scrollRequest();
		}

		m_refIndex = refIndex;
		updateRefLabels();

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
	updateRefLabels();
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

	// look for local references again, in case file content was not present when local symbols were activated
	if (m_activeLocalTokenIds.size() && !m_localReferences.size())
	{
		setActiveLocalTokenIds(utility::toVector(m_activeLocalTokenIds), LOCATION_TOKEN);
		updateRefLabels();
	}
}

size_t QtCodeNavigator::findScreenMatches(const std::wstring& query)
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
		updateRefLabels();
	}

	if (m_activeReference.tokenId)
	{
		if (m_mode == MODE_LIST)
		{
			m_list->requestFileContent(m_activeReference.filePath);
		}

		requestScroll(m_activeReference.filePath, 0, m_activeReference.locationId, true, QtCodeNavigateable::SCROLL_CENTER);
		emit scrollRequest();

		updateRefLabels();
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

void QtCodeNavigator::previousFile(bool fromUI)
{
	if (!m_references.size())
	{
		return;
	}

	if (m_refIndex == 0)
	{
		m_refIndex = m_references.size();
	}
	else
	{
		const Reference& ref = m_references[m_refIndex - 1];
		do
		{
			m_refIndex--;
			if (m_refIndex == 0)
			{
				m_refIndex = m_references.size();
			}
		}
		while (&ref != &m_references[m_refIndex - 1] && ref.filePath == m_references[m_refIndex - 1].filePath);
	}

	m_activeReference = Reference();

	showCurrentReference(fromUI);
}

void QtCodeNavigator::nextFile(bool fromUI)
{
	if (!m_references.size())
	{
		return;
	}

	if (m_refIndex == 0)
	{
		m_refIndex++;
	}
	else
	{
		const Reference& ref = m_references[m_refIndex - 1];
		do
		{
			if (m_refIndex == m_references.size())
			{
				m_refIndex = 0;
			}
			m_refIndex++;
		}
		while (&ref != &m_references[m_refIndex - 1] && ref.filePath == m_references[m_refIndex - 1].filePath);
	}

	m_activeReference = Reference();

	showCurrentReference(fromUI);
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

void QtCodeNavigator::previousLocalReference(bool fromUI)
{
	if (!m_localReferences.size())
	{
		return;
	}

	if (m_localRefIndex < 2)
	{
		m_localRefIndex = m_localReferences.size();
	}
	else
	{
		m_localRefIndex--;
	}

	showCurrentLocalReference();
}

void QtCodeNavigator::nextLocalReference(bool fromUI)
{
	if (!m_localReferences.size())
	{
		return;
	}

	m_localRefIndex++;

	if (m_localRefIndex == m_localReferences.size() + 1)
	{
		m_localRefIndex = 1;
	}

	showCurrentLocalReference();
}

void QtCodeNavigator::setModeList()
{
	if (m_mode == MODE_LIST)
	{
		return;
	}

	m_single->clickedSnippetButton();
}

void QtCodeNavigator::setModeSingle()
{
	if (m_mode == MODE_SINGLE)
	{
		return;
	}

	QtCodeFile* file = m_list->getFirstFileWithActiveLocationId().first;
	if (file)
	{
		file->clickedMaximizeButton();
	}
	else
	{
		m_list->maximizeFirstFile();
	}
}

void QtCodeNavigator::showCurrentReference(bool fromUI)
{
	const Reference& ref = m_references[m_refIndex - 1];
	MessageShowReference(m_refIndex, ref.tokenId, ref.locationId, fromUI).dispatch();
}


void QtCodeNavigator::showCurrentLocalReference()
{
	if (m_localRefIndex > 0)
	{
		const Reference& ref = m_localReferences[m_localRefIndex - 1];
		setCurrentActiveLocalLocationIds({ ref.locationId });

		if (ref.locationType == LOCATION_TOKEN)
		{
			setCurrentActiveTokenIds({ });

			// synchronise reference navigation with local reference navigation
			for (size_t i = 0; i < m_references.size(); i++)
			{
				if (m_references[i].locationId == ref.locationId)
				{
					m_refIndex = i + 1;
				}
			}
		}

		updateFiles();

		requestScroll(ref.filePath, 0, ref.locationId, true, QtCodeNavigateable::SCROLL_CENTER);
		emit scrollRequest();
	}
}

void QtCodeNavigator::updateRefLabels()
{
	size_t refCount = m_references.size();

	// std::set<FilePath> files;
	// size_t fileIndex = 0;
	// for (size_t i = 0; i < m_references.size(); i++)
	// {
	// 	const Reference& ref = m_references[i];
	// 	files.insert(ref.filePath);
	// 	if (i == m_refIndex - 1)
	// 	{
	// 		fileIndex = files.size();
	// 	}
	// }

	if (m_refIndex)
	{
		m_refLabel->setText(QString::number(m_refIndex) + "/" + QString::number(refCount) + " references");
		// m_refLabel->setText(
		// 	QString::number(fileIndex) + "/" + QString::number(files.size()) + " files  |  " +
		// 	QString::number(m_refIndex) + "/" + QString::number(refCount) + " references"
		// );
	}
	else
	{
		m_refLabel->setText(QString::number(refCount) + " references");
		// m_refLabel->setText(QString::number(files.size()) + " files  |  " + QString::number(refCount) + " references");
	}

	m_refLabel->setMinimumWidth(
		m_refLabel->fontMetrics().width(QString(QString::number(refCount).size() * 2, 'a') + "/ references") + 30);

	m_prevFileButton->setEnabled(refCount > 1);
	m_nextFileButton->setEnabled(refCount > 1);
	m_prevReferenceButton->setEnabled(refCount > 1);
	m_nextReferenceButton->setEnabled(refCount > 1);


	size_t localRefCount = m_localReferences.size();
	if (m_localRefIndex)
	{
		m_localRefLabel->setText(QString::number(m_localRefIndex) + "/" + QString::number(localRefCount) + " local references");
	}
	else
	{
		m_localRefLabel->setText(QString::number(localRefCount) + " local references");
	}

	m_localRefLabel->setMinimumWidth(
		m_localRefLabel->fontMetrics().width(QString(QString::number(localRefCount).size() * 2, 'a') + "/ local references") + 30);

	m_nextLocalReferenceButton->setVisible(localRefCount > 1);
	m_prevLocalReferenceButton->setVisible(localRefCount > 1);
	m_localRefLabel->setVisible(localRefCount > 1);
}

void QtCodeNavigator::handleMessage(MessageCodeReference* message)
{
	bool next = (message->type == MessageCodeReference::REFERENCE_NEXT);
	bool local = message->localReference;

	m_onQtThread(
		[=]()
		{
			if (next && local)
			{
				nextLocalReference();
			}
			else if (next)
			{
				nextReference();
			}
			else if (local)
			{
				previousLocalReference();
			}
			else
			{
				previousReference();
			}
		}
	);
}

void QtCodeNavigator::handleMessage(MessageIndexingFinished* message)
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
				setCurrentActiveLocationIds({ ref.locationId });

				if (!replayed)
				{
					updateFiles();
				}

				requestScroll(ref.filePath, 0, ref.locationId, true, QtCodeNavigateable::SCROLL_CENTER);
				emit scrollRequest();

				if (ref.locationType == LOCATION_ERROR)
				{
					MessageShowError(ref.tokenId).dispatch();
				}
			}

			if (!replayed)
			{
				updateRefLabels();
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
