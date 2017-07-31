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
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeSnippet.h"
#include "qt/utility/utilityQt.h"
#include "settings/ApplicationSettings.h"

QtCodeNavigator::QtCodeNavigator(QWidget* parent)
	: QWidget(parent)
	, m_mode(MODE_NONE)
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

		connect(m_prevButton, SIGNAL(clicked()), this, SLOT(previousReference()));
		connect(m_nextButton, SIGNAL(clicked()), this, SLOT(nextReference()));


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

		connect(m_listButton, SIGNAL(clicked()), this, SLOT(setModeList()));
		connect(m_fileButton, SIGNAL(clicked()), this, SLOT(setModeSingle()));


		navigation->setLayout(navLayout);
		layout->addWidget(navigation);

		refreshStyle();

		QHBoxLayout* separatorLayout = new QHBoxLayout();

		QWidget* widget = new QWidget();
		widget->setObjectName("separator_gap");
		widget->setFixedWidth(7);
		separatorLayout->addWidget(widget);

		m_separatorLine = new QFrame();
		m_separatorLine->setFrameShape(QFrame::HLine);
		m_separatorLine->setFrameShadow(QFrame::Plain);
		m_separatorLine->setObjectName("separator_line");
		m_separatorLine->setFixedHeight(1);
		m_separatorLine->hide();
		separatorLayout->addWidget(m_separatorLine);

		QWidget* widget2 = new QWidget();
		widget2->setObjectName("separator_gap");
		widget2->setFixedWidth(7);
		separatorLayout->addWidget(widget2);

		layout->addLayout(separatorLayout);
	}

	m_list = new QtCodeFileList(this);
	layout->addWidget(m_list);

	m_single = new QtCodeFileSingle(this);
	layout->addWidget(m_single);

	if (ApplicationSettings::getInstance()->getCodeViewModeSingle())
	{
		setModeSingle();
	}
	else
	{
		setModeList();
	}

	connect(this, SIGNAL(scrollRequest()), this, SLOT(handleScrollRequest()), Qt::QueuedConnection);
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

void QtCodeNavigator::addFile(std::shared_ptr<SourceLocationFile> locationFile, int refCount, TimePoint modificationTime)
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
}

void QtCodeNavigator::clearFile()
{
	m_single->clearFile();
}

void QtCodeNavigator::clearCaches()
{
	m_single->clearCache();
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
		if (error.filePath == filePath && error.fatal)
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

	if (firstReference.tokenId)
	{
		if (scrollTo)
		{
			requestScroll(firstReference.filePath, 0, firstReference.locationId, true, false);
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

void QtCodeNavigator::updateFiles()
{
	m_current->updateFiles();
}

void QtCodeNavigator::showContents()
{
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
		ResourcePaths::getGuiPath().str() + "code_view/images/arrow_left.png",
		"search/button"
	));

	m_nextButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "code_view/images/arrow_right.png",
		"search/button"
	));

	m_listButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "code_view/images/list.png",
		"search/button"
	));

	m_fileButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "code_view/images/file.png",
		"search/button"
	));

	m_prevButton->setIconSize(QSize(12, 12));
	m_nextButton->setIconSize(QSize(12, 12));
	m_listButton->setIconSize(QSize(14, 14));
	m_fileButton->setIconSize(QSize(14, 14));
}

void QtCodeNavigator::scrollToValue(int value, bool inListMode)
{
	if ((m_mode == MODE_LIST) == inListMode)
	{
		m_value = value;
		QTimer::singleShot(100, this, SLOT(setValue()));
		m_scrollRequest = ScrollRequest();
	}
}

void QtCodeNavigator::scrollToLine(const FilePath& filePath, unsigned int line)
{
	requestScroll(filePath, line, 0, false, false);
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

		requestScroll(m_activeReference.filePath, 0, m_activeReference.locationId, true, false);
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
			requestScroll(m_references.front().filePath, 0, m_references.front().locationId, false, false);
			emit scrollRequest();
		}
		return;
	}

	if (m_mode == MODE_LIST)
	{
		std::pair<QtCodeSnippet*, Id> result = m_list->getFirstSnippetWithActiveLocationId(m_activeTokenId);
		if (result.first != nullptr)
		{
			requestScroll(result.first->getFile()->getFilePath(), 0, result.second, animated, false);
			emit scrollRequest();
			return;
		}
	}
	else
	{
		Id locationId = m_single->getLocationIdOfFirstActiveLocationOfTokenId(m_activeTokenId);
		if (locationId)
		{
			requestScroll(m_single->getCurrentFilePath(), 0, locationId, true, false);
			emit scrollRequest();
			return;
		}
	}

	if (m_references.size())
	{
		m_current->requestFileContent(m_references.front().filePath);
		requestScroll(m_references.front().filePath, 0, m_references.front().locationId, false, false);
		emit scrollRequest();
	}
}

void QtCodeNavigator::scrollToSnippetIfRequested()
{
	emit scrollRequest();
}

void QtCodeNavigator::requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, bool onTop)
{
	ScrollRequest req;
	req.filePath = filePath;
	req.lineNumber = lineNumber;
	req.locationId = locationId;
	req.animated = animated;
	req.onTop = onTop;

	if (m_mode == MODE_SINGLE)
	{
		if (req.lineNumber || m_singleHasNewFile)
		{
			req.animated = false;
		}
	}

	// std::cout << "scroll request: " << req.filePath.str() << " " << req.lineNumber << " " << req.locationId;
	// std::cout << " " << req.animated << " " << req.onTop << std::endl;

	if ((!m_scrollRequest.lineNumber || !m_scrollRequest.locationId) && (req.lineNumber || req.locationId))
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

	bool done = m_current->requestScroll(req.filePath, req.lineNumber, req.locationId, req.animated, req.onTop);
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
	setMode(MODE_LIST);
}

void QtCodeNavigator::setModeSingle()
{
	setMode(MODE_SINGLE);
}

void QtCodeNavigator::setMode(Mode mode)
{
	m_listButton->setChecked(mode == MODE_LIST);
	m_fileButton->setChecked(mode == MODE_SINGLE);

	if (m_mode == mode)
	{
		return;
	}

	m_mode = mode;

	switch (mode)
	{
		case MODE_SINGLE:
			m_list->hide();
			m_single->show();
			m_separatorLine->hide();
			m_current = m_single;
			break;
		case MODE_LIST:
			m_single->hide();
			m_list->show();
			m_separatorLine->show();
			m_current = m_list;
			break;
		default:
			LOG_ERROR("Wrong mode set in code navigator");
			return;
	}

	ApplicationSettings::getInstance()->setCodeViewModeSingle(m_mode == MODE_SINGLE);
	ApplicationSettings::getInstance()->save();

	scrollToDefinition(false, false);
	showContents();
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
	m_refIndex = message->refIndex;

	m_onQtThread(
		[=]()
		{
			if (m_refIndex > 0)
			{
				const Reference& ref = m_references[m_refIndex - 1];
				setCurrentActiveLocationIds(std::vector<Id>(1, ref.locationId));
				updateFiles();

				requestScroll(ref.filePath, 0, ref.locationId, true, false);
				emit scrollRequest();

				if (ref.locationType == LOCATION_ERROR)
				{
					MessageShowErrors(ref.tokenId).dispatch();
				}
			}

			updateRefLabel();
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
