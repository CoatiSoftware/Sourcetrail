#include "QtCodeFileSingle.h"

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

#include "FilePath.h"
#include "MessageChangeFileView.h"
#include "QtCodeArea.h"
#include "QtCodeFileTitleBar.h"
#include "QtCodeFileTitleButton.h"
#include "QtCodeNavigator.h"
#include "SourceLocationFile.h"
#include "logging.h"
#include "utilityQt.h"

QtCodeFileSingle::QtCodeFileSingle(QtCodeNavigator* navigator, QWidget* parent)
	: m_navigator(navigator), m_area(nullptr)
{
	setObjectName(QStringLiteral("code_container"));

	setLayout(new QVBoxLayout(this));
	layout()->setContentsMargins(0, 0, 0, 0);
	layout()->setSpacing(0);

	m_titleBar = new QtCodeFileTitleBar(this, false, true);
	m_titleBar->setObjectName(QStringLiteral("title_bar_single"));
	layout()->addWidget(m_titleBar);

	connect(m_titleBar, &QtCodeFileTitleBar::snippet, this, &QtCodeFileSingle::clickedSnippetButton);

	m_areaWrapper = new QWidget();
	m_areaWrapper->setObjectName(QStringLiteral("code_file_single"));
	m_areaWrapper->setSizePolicy(
		m_areaWrapper->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
	m_areaWrapper->setLayout(new QVBoxLayout());
	m_areaWrapper->layout()->setMargin(0);
	m_areaWrapper->layout()->setSpacing(0);
	layout()->addWidget(m_areaWrapper);
}

void QtCodeFileSingle::clearFile()
{
	setFileData(FileData());
}

void QtCodeFileSingle::clearCache()
{
	clearFile();

	for (auto& p: m_fileDatas)
	{
		p.second.area->deleteLater();
	}

	m_fileDatas.clear();
	m_filePaths.clear();

	m_lastLocationFile.reset();
}

bool QtCodeFileSingle::addFile(const CodeFileParams& params, bool useSingleFileCache)
{
	if (!params.fileParams)
	{
		LOG_ERROR("File params have missing information.");
		return false;
	}

	std::shared_ptr<SourceLocationFile> locationFile = params.fileParams->locationFile;

	FileData file = useSingleFileCache ? getFileData(locationFile->getFilePath()) : FileData();
	if (file.area)
	{
		if (file.area == m_area)
		{
			return false;
		}

		setFileData(file);
		return true;
	}

	if (!locationFile->isWhole())
	{
		LOG_ERROR("Snippet params passed are not for whole file.");
		return false;
	}

	// prevent non cached file from being created again when currently displayed
	if (locationFile == m_lastLocationFile && locationFile->getFilePath() == m_currentFilePath)
	{
		return false;
	}
	m_lastLocationFile = locationFile;

	file.filePath = locationFile->getFilePath();
	file.isComplete = locationFile->isComplete();
	file.isIndexed = locationFile->isIndexed();
	file.modificationTime = params.modificationTime;

	if (params.fileParams->isOverview)
	{
		file.title = params.fileParams->title;
	}

	file.area = new QtCodeArea(
		1, params.fileParams->code, locationFile, m_navigator, !params.fileParams->isOverview, this);
	connect(
		file.area->verticalScrollBar(),
		&QScrollBar::valueChanged,
		m_navigator,
		&QtCodeNavigator::scrolled);

	setFileData(file);
	updateRefCount(params.referenceCount);

	if (useSingleFileCache)
	{
		m_fileDatas.emplace(file.filePath, file);
		m_filePaths.push_back(file.filePath);

		if (m_filePaths.size() > 100)
		{
			// TODO: don't delete files that were added multiple times
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

	return true;
}

QAbstractScrollArea* QtCodeFileSingle::getScrollArea()
{
	return m_area;
}

void QtCodeFileSingle::updateSourceLocations(const CodeSnippetParams& params)
{
	if (m_currentFilePath == params.locationFile->getFilePath())
	{
		m_area->updateSourceLocations(params.locationFile);
	}
}

void QtCodeFileSingle::updateFiles()
{
	if (m_area)
	{
		m_area->updateContent();
		m_area->show();

		// Resizes the area to fix a bug where the area could be scrolled below the last line.
		m_area->updateGeometry();
	}
}

void QtCodeFileSingle::scrollTo(
	const FilePath& filePath,
	size_t lineNumber,
	Id locationId,
	bool animated,
	CodeScrollParams::Target target)
{
	if (m_currentFilePath != filePath)
	{
		FileData file = getFileData(filePath);
		if (!file.area)
		{
			return;
		}

		setFileData(file);
		animated = false;
	}

	size_t endLineNumber = 0;
	if (!lineNumber)
	{
		if (locationId)
		{
			std::pair<size_t, size_t> lineNumbers = m_area->getLineNumbersForLocationId(locationId);

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

	m_area->ensureLocationIdVisible(locationId, width(), animated);
}

void QtCodeFileSingle::onWindowFocus()
{
	m_titleBar->getTitleButton()->updateTexts();
}

void QtCodeFileSingle::findScreenMatches(
	const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
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
	m_navigator->setMode(QtCodeNavigator::MODE_LIST);

	MessageChangeFileView(
		m_currentFilePath,
		MessageChangeFileView::FILE_SNIPPETS,
		MessageChangeFileView::VIEW_LIST,
		CodeScrollParams::toFile(m_currentFilePath, CodeScrollParams::Target::TOP),
		true)
		.dispatch();
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
			m_titleBar->setIsComplete(true);
			m_titleBar->setIsIndexed(true);
		}
		else
		{
			titleButton->setFilePath(file.filePath);
			titleButton->setModificationTime(file.modificationTime);
			m_titleBar->setIsComplete(file.isComplete);
			m_titleBar->setIsIndexed(file.isIndexed);
		}

		updateRefCount(m_area->getActiveLocationCount());

		titleButton->updateTexts();
		titleButton->show();
		m_area->show();

		// Resizes the area to fix a bug where the area could be scrolled below the last line.
		m_area->updateGeometry();
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
	size_t fatalErrorCount = hasErrors ? m_navigator->getFatalErrorCountForFile(m_currentFilePath)
									   : 0;

	m_titleBar->updateRefCount(refCount, hasErrors, fatalErrorCount);
}
