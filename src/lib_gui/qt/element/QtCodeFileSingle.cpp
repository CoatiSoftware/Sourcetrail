#include "qt/element/QtCodeFileSingle.h"

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

#include "utility/file/FilePath.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageChangeFileView.h"

#include "data/location/SourceLocationFile.h"
#include "qt/element/QtCodeArea.h"
#include "qt/element/QtCodeFileTitleBar.h"
#include "qt/element/QtCodeFileTitleButton.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/utility/utilityQt.h"

QtCodeFileSingle::QtCodeFileSingle(QtCodeNavigator* navigator, QWidget* parent)
	: m_navigator(navigator)
	, m_area(nullptr)
	, m_contentRequested(false)
	, m_scrollRequested(false)
{
	setObjectName("code_container");

	setLayout(new QVBoxLayout(this));
	layout()->setContentsMargins(0, 0, 0, 0);
	layout()->setSpacing(0);

	m_titleBar = new QtCodeFileTitleBar(this, false, true);
	m_titleBar->setObjectName("title_bar_single");
	layout()->addWidget(m_titleBar);

	connect(m_titleBar, &QtCodeFileTitleBar::snippet, this, &QtCodeFileSingle::clickedSnippetButton);

	m_areaWrapper = new QWidget();
	m_areaWrapper->setObjectName("code_file_single");
	m_areaWrapper->setSizePolicy(m_areaWrapper->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
	m_areaWrapper->setLayout(new QVBoxLayout());
	m_areaWrapper->layout()->setMargin(0);
	m_areaWrapper->layout()->setSpacing(0);
	layout()->addWidget(m_areaWrapper);
}

QtCodeFileSingle::~QtCodeFileSingle()
{
}

QAbstractScrollArea* QtCodeFileSingle::getScrollArea()
{
	return m_area;
}

void QtCodeFileSingle::clearFile()
{
	setFileData(FileData());
}

void QtCodeFileSingle::clearCache()
{
	clearFile();

	for (auto& p : m_fileDatas)
	{
		p.second.area->deleteLater();
	}

	m_fileDatas.clear();
	m_filePaths.clear();
}

void QtCodeFileSingle::addCodeSnippet(const CodeSnippetParams& params)
{
	if (!params.locationFile->isWhole())
	{
		LOG_ERROR("Snippet params passed are not for whole file.");
		return;
	}

	FileData file = getFileData(params.locationFile->getFilePath());
	if (file.area)
	{
		setFileData(file);
		return;
	}

	file.filePath = params.locationFile->getFilePath();
	file.modificationTime = params.modificationTime;
	file.isComplete = params.locationFile->isComplete();

	if (params.reduced)
	{
		file.title = params.title;
	}

	file.area = new QtCodeArea(1, params.code, params.locationFile, m_navigator, !params.reduced, this);
	connect(file.area->verticalScrollBar(), &QScrollBar::valueChanged, m_navigator, &QtCodeNavigator::scrolled);

	m_fileDatas.emplace(file.filePath, file);
	m_filePaths.push_back(file.filePath);

	setFileData(file);

	m_contentRequested = false;

	if (m_filePaths.size() > 100)
	{
		FilePath toDelete = m_filePaths.front();
		m_filePaths.pop_front();

		auto it = m_fileDatas.find(toDelete);
		if (it != m_fileDatas.end())
		{
			it->second.area->deleteLater();
			m_fileDatas.erase(it);
		}
	}
}

void QtCodeFileSingle::requestFileContent(const FilePath& filePath)
{
	if (m_contentRequested)
	{
		return;
	}

	FileData file = getFileData(filePath);
	if (file.area)
	{
		setFileData(file);
		return;
	}

	m_contentRequested = true;

	MessageChangeFileView(
		filePath,
		MessageChangeFileView::FILE_MAXIMIZED,
		MessageChangeFileView::VIEW_SINGLE,
		true,
		m_navigator->hasErrors()
	).dispatch();
}

bool QtCodeFileSingle::requestScroll(
	const FilePath& filePath, uint lineNumber, Id locationId, bool animated, ScrollTarget target)
{
	FileData file = getFileData(filePath);
	if (file.area)
	{
		setFileData(file);
	}
	else
	{
		requestFileContent(filePath);
		return false;
	}

	if (!m_scrollRequested)
	{
		animated = false;
	}

	uint endLineNumber = 0;
	if (!lineNumber)
	{
		if (locationId)
		{
			std::pair<uint, uint> lineNumbers = m_area->getLineNumbersForLocationId(locationId);

			lineNumber = lineNumbers.first;

			if (lineNumbers.first != lineNumbers.second)
			{
				endLineNumber = lineNumbers.second;
			}
		}
		else
		{
			lineNumber = 1;
		}
	}

	double percentA = double(lineNumber - 1) / m_area->getEndLineNumber();
	double percentB = endLineNumber ? double(endLineNumber - 1) / m_area->getEndLineNumber() : 0.0f;

	ensurePercentVisibleAnimated(percentA, percentB, animated, target);

	m_scrollRequested = true;

	return true;
}

void QtCodeFileSingle::updateFiles()
{
	if (m_area)
	{
		m_area->updateContent();
	}
}

void QtCodeFileSingle::showContents()
{
	if (m_area)
	{
		m_area->show();
		updateRefCount(m_area->getActiveLocationCount());
	}
}

void QtCodeFileSingle::onWindowFocus()
{
	m_titleBar->getTitleButton()->updateTexts();
}

void QtCodeFileSingle::findScreenMatches(const std::string& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	if (m_area)
	{
		m_area->findScreenMatches(query, screenMatches);
	}
}

const FilePath& QtCodeFileSingle::getCurrentFilePath() const
{
	return m_currentFilePath;
}

bool QtCodeFileSingle::hasFileCached(const FilePath& filePath) const
{
	return getFileData(filePath).area != nullptr;
}

Id QtCodeFileSingle::getLocationIdOfFirstActiveLocationOfTokenId(Id tokenId) const
{
	if (!m_area)
	{
		return 0;
	}

	Id scopeId = m_area->getLocationIdOfFirstActiveScopeLocation(tokenId);
	if (scopeId)
	{
		return scopeId;
	}

	return m_area->getLocationIdOfFirstActiveLocation(tokenId);
}

void QtCodeFileSingle::clickedSnippetButton()
{
	m_navigator->requestScroll(m_currentFilePath, 0, 0, false, QtCodeNavigateable::SCROLL_TOP);

	MessageChangeFileView(
		m_currentFilePath,
		MessageChangeFileView::FILE_SNIPPETS,
		MessageChangeFileView::VIEW_LIST,
		true, // TODO: check if data is really needed
		m_navigator->hasErrors(),
		true
	).dispatch();
}

QtCodeFileSingle::FileData QtCodeFileSingle::getFileData(const FilePath& filePath) const
{
	std::map<FilePath, FileData>::const_iterator it = m_fileDatas.find(filePath);
	if (it != m_fileDatas.end())
	{
		return it->second;
	}

	return FileData();
}

void QtCodeFileSingle::setFileData(const FileData& file)
{
	if (file.area == m_area)
	{
		return;
	}

	if (m_area)
	{
		m_area->hide();
		m_area = nullptr;
		m_currentFilePath = FilePath();
	}

	m_areaWrapper->layout()->takeAt(0);

	QtCodeFileTitleButton* titleButton = m_titleBar->getTitleButton();
	if (file.area)
	{
		m_area = file.area;
		m_area->setSizePolicy(m_area->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
		m_areaWrapper->layout()->addWidget(m_area);
		m_area->updateContent();

		m_currentFilePath = file.filePath;
		m_titleBar->setMaximized();

		if (!file.title.empty())
		{
			titleButton->setProject(file.title);
			titleButton->setIsComplete(true);
		}
		else
		{
			titleButton->setFilePath(file.filePath);
			titleButton->setModificationTime(file.modificationTime);
			titleButton->setIsComplete(file.isComplete);
		}

		updateRefCount(m_area->getActiveLocationCount());

		titleButton->updateTexts();
		titleButton->show();
		m_area->show();

		m_scrollRequested = false;
	}
	else
	{
		titleButton->hide();
		updateRefCount(0);
		m_titleBar->setSnippets();
	}
}

void QtCodeFileSingle::updateRefCount(int refCount)
{
	bool hasErrors = m_navigator->hasErrors();
	size_t fatalErrorCount = hasErrors ? m_navigator->getFatalErrorCountForFile(m_currentFilePath) : 0;

	m_titleBar->updateRefCount(refCount, hasErrors, fatalErrorCount);
}
