#include "qt/element/QtCodeFileList.h"

#include <QScrollBar>
#include <QVBoxLayout>
#include <QTimer>

#include "utility/file/FilePath.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityApp.h"

#include "data/location/SourceLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeFileTitleBar.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/element/QtCodeSnippet.h"
#include "qt/utility/utilityQt.h"
#include "settings/ColorScheme.h"

QtCodeFileList::QtCodeFileList(QtCodeNavigator* navigator)
	: QFrame()
	, m_navigator(navigator)
	, m_mirroredTitleBar(nullptr)
	, m_mirroredSnippetScrollBar(nullptr)
{
	m_scrollArea = new QScrollArea();

	m_scrollArea->setObjectName("code_container");
	m_scrollArea->setWidgetResizable(true);

	m_filesArea = new QFrame();
	m_filesArea->setObjectName("code_file_list");

	QVBoxLayout* innerLayout = new QVBoxLayout();
	innerLayout->setSpacing(8);
	innerLayout->setContentsMargins(8, 8, 8, 8);
	innerLayout->setAlignment(Qt::AlignTop);
	m_filesArea->setLayout(innerLayout);

	m_scrollArea->setWidget(m_filesArea);

	m_firstSnippetTitleBar = new QtCodeFileTitleBar(m_scrollArea, true);
	m_firstSnippetTitleBar->hide();

	m_lastSnippetScrollBar = new QScrollBar(Qt::Horizontal, m_scrollArea);
	if (utility::getOsType() == OS_MAC)
	{
		// set style on scrollbar because it always has bright background by default
		m_lastSnippetScrollBar->setStyleSheet(
			utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"main/scrollbar.css")).c_str());
		m_styleSize = m_lastSnippetScrollBar->styleSheet().size();
	}
	else
	{
		m_lastSnippetScrollBar->setObjectName("last_scroll_bar");
	}

	m_lastSnippetScrollBar->hide();
	connect(m_lastSnippetScrollBar, &QScrollBar::valueChanged, this, &QtCodeFileList::scrollLastSnippet);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_scrollArea);
	setLayout(layout);

	m_scrollSpeedChangeListener.setScrollBar(m_scrollArea->verticalScrollBar());

	connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &QtCodeFileList::updateSnippetTitleAndScrollBar);
	connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, m_navigator, &QtCodeNavigator::scrolled);
}

QtCodeFileList::~QtCodeFileList()
{
}

void QtCodeFileList::clear()
{
	for (QtCodeFile* file : m_files)
	{
		file->deleteLater();
	}

	m_files.clear();
	m_scrollArea->verticalScrollBar()->setValue(0);

	clearSnippetTitleAndScrollBar();
}

void QtCodeFileList::clearSnippetTitleAndScrollBar()
{
	updateFirstSnippetTitleBar(nullptr);
	updateLastSnippetScrollBar(nullptr);
}

QtCodeFile* QtCodeFileList::getFile(const FilePath filePath)
{
	QtCodeFile* file = nullptr;

	for (QtCodeFile* filePtr : m_files)
	{
		if (filePtr->getFilePath() == filePath)
		{
			file = filePtr;
			break;
		}
	}

	if (!file)
	{
		file = new QtCodeFile(filePath, m_navigator);
		m_files.push_back(file);

		m_filesArea->layout()->addWidget(file);
		file->hide();
	}

	return file;
}

void QtCodeFileList::addFile(const FilePath& filePath, bool isWholeFile, int refCount, TimeStamp modificationTime, bool isComplete)
{
	QtCodeFile* file = getFile(filePath);
	file->setWholeFile(isWholeFile, refCount);
	file->setModificationTime(modificationTime);
	file->setIsComplete(isComplete);
}

QScrollArea* QtCodeFileList::getScrollArea()
{
	return m_scrollArea;
}

void QtCodeFileList::addCodeSnippet(const CodeSnippetParams& params)
{
	QtCodeFile* file = getFile(params.locationFile->getFilePath());

	if (params.insertSnippet)
	{
		file->insertCodeSnippet(params);
	}
	else
	{
		file->addCodeSnippet(params);
	}

	file->setModificationTime(params.modificationTime);
	file->setIsComplete(params.locationFile->isComplete());
}

void QtCodeFileList::updateCodeSnippet(const CodeSnippetParams& params)
{
	QtCodeFile* file = getFile(params.locationFile->getFilePath());
	file->updateCodeSnippet(params);
}

void QtCodeFileList::requestFileContent(const FilePath& filePath)
{
	getFile(filePath)->requestContent();
}

bool QtCodeFileList::requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, ScrollTarget target)
{
	QtCodeFile* file = getFile(filePath);
	if (!file)
	{
		return true;
	}

	if (file->isCollapsed())
	{
		file->requestContent();
		return false;
	}

	QtCodeSnippet* snippet = nullptr;

	if (locationId)
	{
		snippet = file->getSnippetForLocationId(locationId);
	}
	else if (lineNumber)
	{
		snippet = file->getSnippetForLine(lineNumber);
	}
	else
	{
		snippet = file->getSnippetForLine(1);
	}

	if (!snippet)
	{
		ensureWidgetVisibleAnimated(m_filesArea, file->getTitleBar(), QRect(), animated, target);
		return true;
	}

	if (!snippet->isVisible())
	{
		file->setSnippets();
		return true;
	}

	uint endLineNumber = 0;
	if (!lineNumber)
	{
		if (locationId)
		{
			std::pair<uint, uint> lineNumbers = snippet->getLineNumbersForLocationId(locationId);

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

	QRectF lineRect = snippet->getLineRectForLineNumber(lineNumber);
	if (endLineNumber)
	{
		lineRect |= snippet->getLineRectForLineNumber(endLineNumber);
	}

	ensureWidgetVisibleAnimated(m_filesArea, snippet, lineRect, animated, target);
	return true;
}

void QtCodeFileList::updateFiles()
{
	for (QtCodeFile* file : m_files)
	{
		file->updateContent();
	}

	// Perform delayed so all widgets are already visible
	QTimer::singleShot(100, this, &QtCodeFileList::updateSnippetTitleAndScrollBarSlot);
}

void QtCodeFileList::showContents()
{
	for (QtCodeFile* file : m_files)
	{
		file->updateTitleBar();
		file->show();
	}
}

void QtCodeFileList::onWindowFocus()
{
	for (QtCodeFile* file : m_files)
	{
		file->updateTitleBar();
	}

	if (m_firstSnippetTitleBar->isVisible())
	{
		m_firstSnippetTitleBar->getTitleButton()->updateTexts();
	}
}

void QtCodeFileList::findScreenMatches(const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	for (QtCodeFile* file : m_files)
	{
		file->findScreenMatches(query, screenMatches);
	}
}

void QtCodeFileList::setFileMinimized(const FilePath path)
{
	if (path.empty())
	{
		if (m_files.size())
		{
			m_files[0]->setMinimized();
		}
	}
	else
	{
		getFile(path)->setMinimized();
	}
}

void QtCodeFileList::setFileSnippets(const FilePath path)
{
	if (path.empty())
	{
		if (m_files.size())
		{
			m_files[0]->setSnippets();
		}
	}
	else
	{
		getFile(path)->setSnippets();
	}
}

void QtCodeFileList::setFileMaximized(const FilePath path)
{
	if (path.empty())
	{
		if (m_files.size())
		{
			m_files[0]->setMaximized();
		}
	}
	else
	{
		getFile(path)->setMaximized();
	}
}

void QtCodeFileList::maximizeFirstFile()
{
	if (m_files.size())
	{
		m_files[0]->clickedMaximizeButton();
	}
}

std::pair<QtCodeSnippet*, Id> QtCodeFileList::getFirstSnippetWithActiveLocationId(Id tokenId) const
{
	std::pair<QtCodeSnippet*, Id> result(nullptr, 0);

	for (QtCodeFile* file : m_files)
	{
		if (file->isCollapsed())
		{
			continue;
		}

		result = file->getFirstSnippetWithActiveLocationId(tokenId);
		if (result.first != nullptr)
		{
			break;
		}
	}

	return result;
}

void QtCodeFileList::resizeEvent(QResizeEvent* event)
{
	clearSnippetTitleAndScrollBar();
	updateSnippetTitleAndScrollBar();
}

void QtCodeFileList::updateSnippetTitleAndScrollBarSlot()
{
	updateSnippetTitleAndScrollBar(0);
}

void QtCodeFileList::updateSnippetTitleAndScrollBar(int value)
{
	QtCodeFile* firstFile = nullptr;
	QScrollBar* lastSnippetScrollBar = nullptr;
	int fileTitleBarOffset = 0;

	const QRect visibleRect(-m_filesArea->pos(), m_scrollArea->viewport()->size());

	for (QtCodeFile* file : m_files)
	{
		QRect fileRect = getFocusRectForWidget(file, m_filesArea);

		if (!firstFile && visibleRect.top() > fileRect.top() + 2 &&
			visibleRect.top() < fileRect.bottom() - 10 &&
			file->getVisibleSnippets().size())
		{
			firstFile = file;
			fileTitleBarOffset = std::min(0, fileRect.bottom() - 10 - (visibleRect.top() + m_firstSnippetTitleBar->height()));
		}

		if (visibleRect.bottom() > fileRect.top() && fileRect.bottom() > visibleRect.bottom())
		{
			for (QtCodeSnippet* snippet : file->getVisibleSnippets())
			{
				QScrollBar* scrollbar = snippet->getArea()->horizontalScrollBar();
				if (!scrollbar || scrollbar->minimum() == scrollbar->maximum())
				{
					continue;
				}

				QRect snippetRect = getFocusRectForWidget(snippet, m_filesArea);
				if (visibleRect.bottom() > snippetRect.top() + scrollbar->height() &&
					snippetRect.bottom() - scrollbar->height() > visibleRect.bottom())
				{
					lastSnippetScrollBar = scrollbar;
					break;
				}
			}
		}

		if (lastSnippetScrollBar)
		{
			break;
		}
	}

	updateFirstSnippetTitleBar(firstFile, fileTitleBarOffset);
	updateLastSnippetScrollBar(lastSnippetScrollBar);
}

void QtCodeFileList::scrollLastSnippet(int value)
{
	if (m_mirroredSnippetScrollBar && m_mirroredSnippetScrollBar->value() != value)
	{
		m_mirroredSnippetScrollBar->setValue(value);
	}
}

void QtCodeFileList::scrollLastSnippetScrollBar(int value)
{
	if (m_lastSnippetScrollBar->value() != value)
	{
		m_lastSnippetScrollBar->setValue(value);
	}
}

void QtCodeFileList::updateFirstSnippetTitleBar(const QtCodeFile* file, int fileTitleBarOffset)
{
	const QtCodeFileTitleBar* mirroredTitleBar = file ? file->getTitleBar() : nullptr;
	if (m_mirroredTitleBar != mirroredTitleBar)
	{
		m_mirroredTitleBar = mirroredTitleBar;

		if (m_mirroredTitleBar && file)
		{
			m_firstSnippetTitleBar->updateFromOther(mirroredTitleBar);

			connect(m_firstSnippetTitleBar, &QtCodeFileTitleBar::minimize, file, &QtCodeFile::clickedMinimizeButton);
			connect(m_firstSnippetTitleBar, &QtCodeFileTitleBar::snippet, file, &QtCodeFile::clickedSnippetButton);
			connect(m_firstSnippetTitleBar, &QtCodeFileTitleBar::maximize, file, &QtCodeFile::clickedMaximizeButton);

			m_firstSnippetTitleBar->setGeometry(
				file->pos().x() + mirroredTitleBar->pos().x(),
				0,
				mirroredTitleBar->width(),
				mirroredTitleBar->height()
			);
			m_firstSnippetTitleBar->show();
		}
		else
		{
			// Forces the title button icon to get reloaded next time, which fixes a color change issue when changing
			// color scheme
			m_firstSnippetTitleBar->getTitleButton()->setFilePath(FilePath());
			m_firstSnippetTitleBar->hide();
		}
	}

	if (m_firstSnippetTitleBar->isVisible())
	{
		QRect rect = m_firstSnippetTitleBar->geometry();
		int height = rect.height();
		if (fileTitleBarOffset != rect.y())
		{
			rect.setY(fileTitleBarOffset);
			rect.setHeight(height);
			m_firstSnippetTitleBar->setGeometry(rect);
			m_firstSnippetTitleBar->update();
		}
	}
}

void QtCodeFileList::updateLastSnippetScrollBar(QScrollBar* mirroredScrollBar)
{
	if (m_mirroredSnippetScrollBar != mirroredScrollBar)
	{
		if (m_mirroredSnippetScrollBar)
		{
			m_mirroredSnippetScrollBar->disconnect(this);
		}

		m_mirroredSnippetScrollBar = mirroredScrollBar;

		if (mirroredScrollBar)
		{
			connect(mirroredScrollBar, &QScrollBar::valueChanged, this, &QtCodeFileList::scrollLastSnippetScrollBar);

			m_lastSnippetScrollBar->setMinimum(mirroredScrollBar->minimum());
			m_lastSnippetScrollBar->setMaximum(mirroredScrollBar->maximum());
			m_lastSnippetScrollBar->setValue(mirroredScrollBar->value());
			m_lastSnippetScrollBar->setPageStep(mirroredScrollBar->pageStep());
			m_lastSnippetScrollBar->setSingleStep(mirroredScrollBar->singleStep());

			m_lastSnippetScrollBar->setGeometry(
				mirroredScrollBar->mapTo(m_scrollArea, mirroredScrollBar->pos()).x(),
				m_scrollArea->viewport()->size().height() - mirroredScrollBar->height(),
				mirroredScrollBar->width(),
				mirroredScrollBar->height()
			);
			m_lastSnippetScrollBar->show();

			if (utility::getOsType() == OS_MAC)
			{
				// set style on scrollbar because it always has bright background by default
				QString style = m_lastSnippetScrollBar->styleSheet();
				style.resize(m_styleSize);
				m_lastSnippetScrollBar->setStyleSheet(style + (
					"\nQScrollBar:horizontal { background: " + ColorScheme::getInstance()->getColor("code/snippet/background") + "; }"
				).c_str());
			}
		}
		else
		{
			m_lastSnippetScrollBar->hide();
		}
	}
}
