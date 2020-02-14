#include "QtCodeNavigator.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "ApplicationSettings.h"
#include "MessageCodeReference.h"
#include "MessageScrollCode.h"
#include "MessageShowError.h"
#include "QtCodeArea.h"
#include "QtCodeFile.h"
#include "QtCodeSnippet.h"
#include "QtSearchBarButton.h"
#include "ResourcePaths.h"
#include "SourceLocation.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"
#include "TabId.h"
#include "logging.h"
#include "utility.h"
#include "utilityQt.h"

QtCodeNavigator::QtCodeNavigator(QWidget* parent)
	: QWidget(parent), m_mode(MODE_NONE), m_oldMode(MODE_NONE), m_schedulerId(TabId::ignore())
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	{
		QWidget* navigation = new QWidget();
		navigation->setObjectName(QStringLiteral("code_navigation"));

		QHBoxLayout* navLayout = new QHBoxLayout();
		navLayout->setSpacing(2);
		navLayout->setContentsMargins(7, 7, 7, 6);

		{
			m_prevReferenceButton = new QtSearchBarButton(
				ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_up.png"), true);
			m_nextReferenceButton = new QtSearchBarButton(
				ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_down.png"), true);

			m_prevReferenceButton->setObjectName(QStringLiteral("reference_button_previous"));
			m_nextReferenceButton->setObjectName(QStringLiteral("reference_button_next"));

			m_prevReferenceButton->setToolTip(QStringLiteral("previous reference"));
			m_nextReferenceButton->setToolTip(QStringLiteral("next reference"));

			m_prevReferenceButton->setIconSize(QSize(12, 12));
			m_nextReferenceButton->setIconSize(QSize(12, 12));

			navLayout->addWidget(m_prevReferenceButton);
			navLayout->addWidget(m_nextReferenceButton);

			connect(
				m_prevReferenceButton, &QPushButton::clicked, this, &QtCodeNavigator::previousReference);
			connect(
				m_nextReferenceButton, &QPushButton::clicked, this, &QtCodeNavigator::nextReference);

			// m_refLabel = new QLabel("0 files  |  0 references");
			m_refLabel = new QLabel(QStringLiteral("0 references"));
			m_refLabel->setObjectName(QStringLiteral("references_label"));
			navLayout->addWidget(m_refLabel);

			navLayout->addStretch();
		}

		{
			m_prevLocalReferenceButton = new QtSearchBarButton(
				ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_up.png"), true);
			m_nextLocalReferenceButton = new QtSearchBarButton(
				ResourcePaths::getGuiPath().concatenate(L"code_view/images/arrow_down.png"), true);

			m_prevLocalReferenceButton->setObjectName(
				QStringLiteral("local_reference_button_previous"));
			m_nextLocalReferenceButton->setObjectName(
				QStringLiteral("local_reference_button_next"));

			m_prevLocalReferenceButton->setToolTip(QStringLiteral("previous local reference"));
			m_nextLocalReferenceButton->setToolTip(QStringLiteral("next local reference"));

			m_prevLocalReferenceButton->setIconSize(QSize(12, 12));
			m_nextLocalReferenceButton->setIconSize(QSize(12, 12));

			navLayout->addWidget(m_prevLocalReferenceButton);
			navLayout->addWidget(m_nextLocalReferenceButton);

			connect(
				m_prevLocalReferenceButton,
				&QPushButton::clicked,
				this,
				&QtCodeNavigator::previousLocalReference);
			connect(
				m_nextLocalReferenceButton,
				&QPushButton::clicked,
				this,
				&QtCodeNavigator::nextLocalReference);

			m_localRefLabel = new QLabel(QStringLiteral("0/0 local references"));
			m_localRefLabel->setObjectName(QStringLiteral("references_label"));
			navLayout->addWidget(m_localRefLabel);

			navLayout->addStretch();

			utility::setWidgetRetainsSpaceWhenHidden(m_prevLocalReferenceButton);
			utility::setWidgetRetainsSpaceWhenHidden(m_nextLocalReferenceButton);
			utility::setWidgetRetainsSpaceWhenHidden(m_localRefLabel);

			m_prevLocalReferenceButton->hide();
			m_nextLocalReferenceButton->hide();
			m_localRefLabel->hide();
		}

		m_listButton = new QtSearchBarButton(
			ResourcePaths::getGuiPath().concatenate(L"code_view/images/list.png"), true);
		m_fileButton = new QtSearchBarButton(
			ResourcePaths::getGuiPath().concatenate(L"code_view/images/file.png"), true);

		m_listButton->setObjectName(QStringLiteral("mode_button_list"));
		m_fileButton->setObjectName(QStringLiteral("mode_button_single"));

		m_listButton->setToolTip(QStringLiteral("snippet list mode"));
		m_fileButton->setToolTip(QStringLiteral("single file mode"));

		m_listButton->setCheckable(true);
		m_fileButton->setCheckable(true);

		m_listButton->setIconSize(QSize(14, 14));
		m_fileButton->setIconSize(QSize(14, 14));

		navLayout->addWidget(m_listButton);
		navLayout->addWidget(m_fileButton);

		connect(m_listButton, &QPushButton::clicked, this, &QtCodeNavigator::setModeList);
		connect(m_fileButton, &QPushButton::clicked, this, &QtCodeNavigator::setModeSingle);

		QButtonGroup* buttonGroup = new QButtonGroup(navigation);
		buttonGroup->addButton(m_listButton);
		buttonGroup->addButton(m_fileButton);

		navigation->setLayout(navLayout);
		layout->addWidget(navigation);

		m_separatorLine = new QFrame();
		m_separatorLine->setFrameShape(QFrame::HLine);
		m_separatorLine->setFrameShadow(QFrame::Plain);
		m_separatorLine->setObjectName(QStringLiteral("separator_line"));
		m_separatorLine->setFixedHeight(1);
		m_separatorLine->hide();
		layout->addWidget(m_separatorLine);
	}

	m_list = new QtCodeFileList(this);
	layout->addWidget(m_list);

	m_single = new QtCodeFileSingle(this);
	layout->addWidget(m_single);

	setMode(ApplicationSettings::getInstance()->getCodeViewModeSingle() ? MODE_SINGLE : MODE_LIST);
	updateFiles();
}

QtCodeNavigator::~QtCodeNavigator() {}

void QtCodeNavigator::addSnippetFile(const CodeFileParams& params)
{
	m_list->addFile(params);
}

bool QtCodeNavigator::addSingleFile(const CodeFileParams& params, bool useSingleFileCache)
{
	return m_single->addFile(params, useSingleFileCache);
}

void QtCodeNavigator::updateSourceLocations(const CodeSnippetParams& params)
{
	m_current->updateSourceLocations(params);
}

void QtCodeNavigator::updateReferenceCount(
	size_t referenceCount, size_t referenceIndex, size_t localReferenceCount, size_t localReferenceIndex)
{
	if (referenceIndex != referenceCount)
	{
		m_refLabel->setText(
			QString::number(referenceIndex + 1) + "/" + QString::number(referenceCount) +
			" references");
	}
	else
	{
		m_refLabel->setText(QString::number(referenceCount) + " references");
	}

	m_refLabel->setMinimumWidth(
		m_refLabel->fontMetrics().width(
			QString(QString::number(referenceCount).size() * 2, 'a') + "/ references") +
		30);

	m_prevReferenceButton->setEnabled(referenceCount > 1);
	m_nextReferenceButton->setEnabled(referenceCount > 1);


	if (localReferenceIndex != localReferenceCount)
	{
		m_localRefLabel->setText(
			QString::number(localReferenceIndex + 1) + "/" + QString::number(localReferenceCount) +
			" local references");
	}
	else
	{
		m_localRefLabel->setText(QString::number(localReferenceCount) + " local references");
	}

	m_localRefLabel->setMinimumWidth(
		m_localRefLabel->fontMetrics().width(
			QString(QString::number(localReferenceCount).size() * 2, 'a') + "/ local references") +
		30);

	m_nextLocalReferenceButton->setVisible(localReferenceCount > 1);
	m_prevLocalReferenceButton->setVisible(localReferenceCount > 1);
	m_localRefLabel->setVisible(localReferenceCount > 1);
}

void QtCodeNavigator::clear()
{
	clearSnippets();
	clearFile();
	clearCache();

	m_currentActiveTokenIds.clear();
	m_activeTokenIds.clear();
	m_activeLocalTokenIds.clear();
	m_focusedTokenIds.clear();
	m_errorInfos.clear();

	updateReferenceCount(0, 0, 0, 0);
}

void QtCodeNavigator::clearSnippets()
{
	clearScreenMatches();
	m_list->clear();
}

void QtCodeNavigator::clearFile()
{
	clearScreenMatches();
	m_single->clearFile();
}

void QtCodeNavigator::clearCache()
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

Id QtCodeNavigator::getSchedulerId() const
{
	return m_schedulerId;
}

void QtCodeNavigator::setSchedulerId(Id schedulerId)
{
	m_schedulerId = schedulerId;
}

const std::set<Id>& QtCodeNavigator::getCurrentActiveTokenIds() const
{
	return m_currentActiveTokenIds;
}

void QtCodeNavigator::setCurrentActiveTokenIds(const std::vector<Id>& currentActiveTokenIds)
{
	m_currentActiveTokenIds = std::set<Id>(
		currentActiveTokenIds.begin(), currentActiveTokenIds.end());
	m_currentActiveLocationIds.clear();
}

const std::set<Id>& QtCodeNavigator::getCurrentActiveLocationIds() const
{
	return m_currentActiveLocationIds;
}

void QtCodeNavigator::setCurrentActiveLocationIds(const std::vector<Id>& currentActiveLocationIds)
{
	setActiveLocalTokenIds({}, LOCATION_TOKEN);

	m_currentActiveLocationIds = std::set<Id>(
		currentActiveLocationIds.begin(), currentActiveLocationIds.end());
	m_currentActiveTokenIds.clear();
}

const std::set<Id>& QtCodeNavigator::getCurrentActiveLocalLocationIds() const
{
	return m_currentActiveLocalLocationIds;
}

void QtCodeNavigator::setCurrentActiveLocalLocationIds(const std::vector<Id>& currentActiveLocalLocationIds)
{
	m_currentActiveLocalLocationIds = std::set<Id>(
		currentActiveLocalLocationIds.begin(), currentActiveLocalLocationIds.end());
}

const std::set<Id>& QtCodeNavigator::getActiveTokenIds() const
{
	return m_activeTokenIds;
}

void QtCodeNavigator::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	setActiveLocalTokenIds({}, LOCATION_TOKEN);
	setCurrentActiveTokenIds(activeTokenIds);

	m_activeTokenIds = std::set<Id>(activeTokenIds.begin(), activeTokenIds.end());
}

const std::set<Id>& QtCodeNavigator::getActiveLocalTokenIds() const
{
	return m_activeLocalTokenIds;
}

void QtCodeNavigator::setActiveLocalTokenIds(
	const std::vector<Id>& activeLocalTokenIds, LocationType locationType)
{
	setCurrentActiveLocalLocationIds({});

	m_activeLocalTokenIds = std::set<Id>(activeLocalTokenIds.begin(), activeLocalTokenIds.end());
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

	for (const ErrorInfo& info: errorInfos)
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
	for (const std::pair<Id, ErrorInfo>& p: m_errorInfos)
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

void QtCodeNavigator::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	setFocusedTokenIds(focusedTokenIds);
	updateFiles();
}

void QtCodeNavigator::defocusTokenIds()
{
	setFocusedTokenIds({});
	updateFiles();
}

void QtCodeNavigator::updateFiles()
{
	m_current->updateFiles();

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

	scrollTo(
		CodeScrollParams::toReference(
			p.first->getSourceLocationFile()->getFilePath(),
			m_activeScreenMatchId,
			CodeScrollParams::Target::CENTER),
		true);
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

bool QtCodeNavigator::hasScreenMatches() const
{
	return !m_screenMatches.empty();
}

void QtCodeNavigator::clearScreenMatches()
{
	if (m_activeScreenMatchId)
	{
		m_currentActiveLocationIds.erase(m_activeScreenMatchId);
		m_activeScreenMatchId = 0;
	}

	for (auto p: m_screenMatches)
	{
		p.first->clearScreenMatches();
	}

	m_screenMatches.clear();
}

void QtCodeNavigator::scrollTo(const CodeScrollParams& params, bool animated)
{
	if (!isVisible())
	{
		m_scrollParams = params;
		return;
	}

	std::function<void()> func = [=]() {};

	switch (params.type)
	{
	case CodeScrollParams::Type::TO_REFERENCE:
		func = [=]() {
			m_current->scrollTo(params.filePath, 0, params.locationId, animated, params.target);
		};
		break;
	case CodeScrollParams::Type::TO_FILE:
		func = [=]() { m_current->scrollTo(params.filePath, 0, 0, animated, params.target); };
		break;
	case CodeScrollParams::Type::TO_LINE:
		func = [=]() {
			m_current->scrollTo(params.filePath, params.line, 0, animated, params.target);
		};
		break;
	case CodeScrollParams::Type::TO_VALUE:
		if ((m_mode == MODE_LIST) == params.inListMode)
		{
			func = [=]() {
				QAbstractScrollArea* area = m_current->getScrollArea();
				if (area)
				{
					area->verticalScrollBar()->setValue(static_cast<int>(params.value));
				}
			};
		}
		break;
	default:
		break;
	}

	if (m_mode == MODE_LIST)
	{
		QTimer::singleShot(100, func);
	}
	else
	{
		func();
	}

	m_scrollParams = CodeScrollParams();
}

void QtCodeNavigator::scrolled(int value)
{
	MessageScrollCode(value, m_mode == MODE_LIST).dispatch();
}

void QtCodeNavigator::showEvent(QShowEvent* event)
{
	scrollTo(m_scrollParams, false);
}

void QtCodeNavigator::previousReference()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_PREVIOUS, false).dispatch();
}

void QtCodeNavigator::nextReference()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_NEXT, false).dispatch();
}

void QtCodeNavigator::previousLocalReference()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_PREVIOUS, true).dispatch();
}

void QtCodeNavigator::nextLocalReference()
{
	MessageCodeReference(MessageCodeReference::REFERENCE_NEXT, true).dispatch();
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

void QtCodeNavigator::handleMessage(MessageWindowFocus* message)
{
	if (message->focusIn)
	{
		m_onQtThread([=]() { m_current->onWindowFocus(); });
	}
}
