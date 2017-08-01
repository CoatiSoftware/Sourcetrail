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

	{
		QWidget* titleBar = new QWidget();
		titleBar->setObjectName("single_file_title_bar");

		QHBoxLayout* titleLayout = new QHBoxLayout();
		titleLayout->setSpacing(0);
		titleLayout->setMargin(0);

		m_title = new QtCodeFileTitleButton();
		m_title->setObjectName("file_title");
		titleLayout->addWidget(m_title);

		m_title->hide();

		m_referenceCount = new QLabel();
		m_referenceCount->setObjectName("references_label");
		m_referenceCount->hide();
		titleLayout->addWidget(m_referenceCount);

		QPushButton* filler = new QPushButton();
		filler->setObjectName("file_title");
		filler->setEnabled(false);
		titleLayout->addWidget(filler);

		titleLayout->addStretch();

		titleBar->setLayout(titleLayout);
		layout()->addWidget(titleBar);
	}

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

	file.area = std::make_shared<QtCodeArea>(1, params.code, params.locationFile, m_navigator, this);
	connect(file.area->verticalScrollBar(), &QScrollBar::valueChanged, m_navigator, &QtCodeNavigator::scrolled);

	m_fileDatas.emplace(file.filePath, file);
	m_filePaths.push_back(file.filePath);

	setFileData(file);

	m_contentRequested = false;

	if (m_filePaths.size() > 100)
	{
		FilePath toDelete = m_filePaths.front();
		m_filePaths.pop_front();

		m_fileDatas.erase(m_fileDatas.find(toDelete));
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
		MessageChangeFileView::FILE_DEFAULT_FOR_MODE,
		true,
		m_navigator->hasErrors()
	).dispatch();
}

bool QtCodeFileSingle::requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, bool onTop)
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

	ensurePercentVisibleAnimated(percentA, percentB, animated, onTop);

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
	}
}

void QtCodeFileSingle::onWindowFocus()
{
	m_title->updateTexts();
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
	if (file.area.get() == m_area)
	{
		if (m_area)
		{
			updateRefCount(m_area->getActiveLocationCount());
			m_area->updateContent();
		}
		return;
	}

	if (m_area)
	{
		m_area->hide();
		m_area = nullptr;
		m_currentFilePath = FilePath();
	}

	m_areaWrapper->layout()->takeAt(0);

	if (file.area)
	{
		m_area = file.area.get();
		m_area->setSizePolicy(m_area->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
		m_areaWrapper->layout()->addWidget(m_area);
		m_area->updateContent();

		m_currentFilePath = file.filePath;

		if (file.title.size())
		{
			m_title->setProject(file.title);
			m_title->setIsComplete(true);
		}
		else
		{
			m_title->setFilePath(file.filePath);
			m_title->setModificationTime(file.modificationTime);
			m_title->setIsComplete(file.isComplete);
		}

		updateRefCount(m_area->getActiveLocationCount());

		m_title->show();
		m_area->show();

		m_scrollRequested = false;
	}
	else
	{
		m_title->hide();
		updateRefCount(0);
	}
}

void QtCodeFileSingle::updateRefCount(int refCount)
{
	if (refCount > 0)
	{
		QString label = m_navigator->hasErrors() ? "error" : "reference";
		if (refCount > 1)
		{
			label += "s";
		}

		size_t fatalErrorCount = m_navigator->getFatalErrorCountForFile(m_currentFilePath);
		if (fatalErrorCount > 0)
		{
			label += " (" + QString::number(fatalErrorCount) + " fatal)";
		}

		m_referenceCount->setText(QString::number(refCount) + " " + label);
		m_referenceCount->show();
	}
	else
	{
		m_referenceCount->hide();
	}
}
