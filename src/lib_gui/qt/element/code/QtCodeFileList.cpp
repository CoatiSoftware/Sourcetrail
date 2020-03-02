#include "QtCodeFileList.h"

#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include "FilePath.h"
#include "ResourcePaths.h"
#include "utility.h"
#include "utilityApp.h"

#include "ColorScheme.h"
#include "QtCodeFile.h"
#include "QtCodeFileTitleBar.h"
#include "QtCodeNavigator.h"
#include "QtCodeSnippet.h"
#include "SourceLocationFile.h"
#include "utilityQt.h"

QtCodeFileList::QtCodeFileList(QtCodeNavigator* navigator)
	: QFrame()
	, m_navigator(navigator)
	, m_mirroredTitleBar(nullptr)
	, m_mirroredSnippetScrollBar(nullptr)
{
	m_scrollArea = new QScrollArea();

	m_scrollArea->setObjectName(QStringLiteral("code_container"));
	m_scrollArea->setWidgetResizable(true);

	m_filesArea = new QFrame();
	m_filesArea->setObjectName(QStringLiteral("code_file_list"));

	QVBoxLayout* innerLayout = new QVBoxLayout();
	innerLayout->setSpacing(0);
	innerLayout->setContentsMargins(0, 0, 0, 0);
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
			utility::getStyleSheet(ResourcePaths::getGuiPath().concatenate(L"main/scrollbar.css"))
				.c_str());
		m_styleSize = m_lastSnippetScrollBar->styleSheet().size();
	}
	else
	{
		m_lastSnippetScrollBar->setObjectName(QStringLiteral("last_scroll_bar"));
	}

	m_lastSnippetScrollBar->hide();
	connect(
		m_lastSnippetScrollBar, &QScrollBar::valueChanged, this, &QtCodeFileList::scrollLastSnippet);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_scrollArea);
	setLayout(layout);

	m_scrollSpeedChangeListener.setScrollBar(m_scrollArea->verticalScrollBar());

	connect(
		m_scrollArea->verticalScrollBar(),
		&QScrollBar::valueChanged,
		this,
		&QtCodeFileList::updateSnippetTitleAndScrollBar);
	connect(
		m_scrollArea->verticalScrollBar(),
		&QScrollBar::valueChanged,
		m_navigator,
		&QtCodeNavigator::scrolled);
}

void QtCodeFileList::clear()
{
	for (QtCodeFile* file: m_files)
	{
		file->hide();
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

QtCodeFile* QtCodeFileList::getFile(const FilePath& filePath)
{
	QtCodeFile* file = nullptr;

	for (QtCodeFile* filePtr: m_files)
	{
		if (filePtr->getFilePath() == filePath)
		{
			file = filePtr;
			break;
		}
	}

	if (!file)
	{
		file = new QtCodeFile(filePath, m_navigator, !m_files.size());
		m_files.push_back(file);

		m_filesArea->layout()->addWidget(file);
		file->hide();
	}

	return file;
}

void QtCodeFileList::addFile(const CodeFileParams& params)
{
	QtCodeFile* file = getFile(params.locationFile->getFilePath());
	file->setWholeFile(params.locationFile->isWhole(), static_cast<int>(params.referenceCount));
	file->setModificationTime(params.modificationTime);
	file->setIsComplete(params.locationFile->isComplete());
	file->setIsIndexed(params.locationFile->isIndexed());

	if (params.isMinimized)
	{
		file->setMinimized();
	}
	else
	{
		bool same = true;
		const std::vector<QtCodeSnippet*> snippets = file->getSnippets();
		if (params.snippetParams.size() != snippets.size())
		{
			same = false;
		}
		else
		{
			for (size_t i = 0; i < snippets.size(); i++)
			{
				if (params.snippetParams[i].startLineNumber != snippets[i]->getStartLineNumber() ||
					params.snippetParams[i].endLineNumber != snippets[i]->getEndLineNumber())
				{
					same = false;
					break;
				}
			}
		}

		if (!same)
		{
			file->clearSnippets();

			for (const CodeSnippetParams& snippetParams: params.snippetParams)
			{
				file->addCodeSnippet(snippetParams);
			}
		}

		file->setSnippets();
	}
}

QScrollArea* QtCodeFileList::getScrollArea()
{
	return m_scrollArea;
}

void QtCodeFileList::updateSourceLocations(const CodeSnippetParams& params)
{
	QtCodeFile* file = getFile(params.locationFile->getFilePath());
	if (file)
	{
		file->updateSourceLocations(params);
	}
}

void QtCodeFileList::updateFiles()
{
	for (QtCodeFile* file: m_files)
	{
		file->setProperty("last", file == m_files.back());
		file->updateContent();
		file->updateTitleBar();
		file->show();
	}

	// Perform delayed so all widgets are already visible
	QTimer::singleShot(100, this, &QtCodeFileList::updateSnippetTitleAndScrollBarSlot);
}

void QtCodeFileList::scrollTo(
	const FilePath& filePath,
	size_t lineNumber,
	Id locationId,
	bool animated,
	CodeScrollParams::Target target)
{
	QtCodeFile* file = getFile(filePath);
	if (!file)
	{
		return;
	}

	QtCodeSnippet* snippet = nullptr;

	if (locationId)
	{
		snippet = file->getSnippetForLocationId(locationId);
	}
	else if (lineNumber)
	{
		snippet = file->getSnippetForLine(static_cast<unsigned int>(lineNumber));
	}
	else
	{
		snippet = file->getSnippetForLine(1);
	}

	if (!snippet || !snippet->isVisible())
	{
		ensureWidgetVisibleAnimated(m_filesArea, file->getTitleBar(), QRect(), animated, target);
		return;
	}

	size_t endLineNumber = 0;
	if (!lineNumber)
	{
		if (locationId)
		{
			std::pair<size_t, size_t> lineNumbers = snippet->getLineNumbersForLocationId(locationId);

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

	snippet->ensureLocationIdVisible(locationId, animated);
}

void QtCodeFileList::onWindowFocus()
{
	for (QtCodeFile* file: m_files)
	{
		file->updateTitleBar();
	}

	if (m_firstSnippetTitleBar->isVisible())
	{
		m_firstSnippetTitleBar->getTitleButton()->updateTexts();
	}
}

void QtCodeFileList::findScreenMatches(
	const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	for (QtCodeFile* file: m_files)
	{
		file->findScreenMatches(query, screenMatches);
	}
}

void QtCodeFileList::maximizeFirstFile()
{
	if (m_files.size())
	{
		m_files[0]->clickedMaximizeButton();
	}
}

std::pair<QtCodeFile*, Id> QtCodeFileList::getFirstFileWithActiveLocationId() const
{
	for (QtCodeFile* file: m_files)
	{
		std::pair<QtCodeSnippet*, Id> snippet = file->getFirstSnippetWithActiveLocationId(0);
		if (snippet.first != nullptr)
		{
			return std::make_pair(file, snippet.second);
		}
	}

	return std::make_pair(nullptr, 0);
}

std::pair<QtCodeSnippet*, Id> QtCodeFileList::getFirstSnippetWithActiveLocationId(Id tokenId) const
{
	std::pair<QtCodeSnippet*, Id> result(nullptr, 0);

	for (QtCodeFile* file: m_files)
	{
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

	for (QtCodeFile* file: m_files)
	{
		QRect fileRect = getFocusRectForWidget(file, m_filesArea);

		if (!firstFile && visibleRect.top() > fileRect.top() &&
			visibleRect.top() < fileRect.bottom() + 1 && file->getVisibleSnippets().size())
		{
			firstFile = file;
			fileTitleBarOffset = std::min(
				0, fileRect.bottom() + 1 - (visibleRect.top() + m_firstSnippetTitleBar->height()));
		}

		if (visibleRect.bottom() > fileRect.top() && fileRect.bottom() > visibleRect.bottom())
		{
			for (QtCodeSnippet* snippet: file->getVisibleSnippets())
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

			connect(
				m_firstSnippetTitleBar,
				&QtCodeFileTitleBar::minimize,
				file,
				&QtCodeFile::clickedMinimizeButton);
			connect(
				m_firstSnippetTitleBar,
				&QtCodeFileTitleBar::snippet,
				file,
				&QtCodeFile::clickedSnippetButton);
			connect(
				m_firstSnippetTitleBar,
				&QtCodeFileTitleBar::maximize,
				file,
				&QtCodeFile::clickedMaximizeButton);

			m_firstSnippetTitleBar->setGeometry(
				file->pos().x() + mirroredTitleBar->pos().x(),
				0,
				mirroredTitleBar->width(),
				mirroredTitleBar->height());
			m_firstSnippetTitleBar->show();
		}
		else
		{
			// Forces the title button icon to get reloaded next time, which fixes a color change
			// issue when changing color scheme
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
			connect(
				mirroredScrollBar,
				&QScrollBar::valueChanged,
				this,
				&QtCodeFileList::scrollLastSnippetScrollBar);

			m_lastSnippetScrollBar->setMinimum(mirroredScrollBar->minimum());
			m_lastSnippetScrollBar->setMaximum(mirroredScrollBar->maximum());
			m_lastSnippetScrollBar->setValue(mirroredScrollBar->value());
			m_lastSnippetScrollBar->setPageStep(mirroredScrollBar->pageStep());
			m_lastSnippetScrollBar->setSingleStep(mirroredScrollBar->singleStep());

			m_lastSnippetScrollBar->setGeometry(
				mirroredScrollBar->mapTo(m_scrollArea, mirroredScrollBar->pos()).x(),
				m_scrollArea->viewport()->size().height() - mirroredScrollBar->height(),
				mirroredScrollBar->width(),
				mirroredScrollBar->height());
			m_lastSnippetScrollBar->show();

			if (utility::getOsType() == OS_MAC)
			{
				// set style on scrollbar because it always has bright background by default
				QString style = m_lastSnippetScrollBar->styleSheet();
				style.resize(m_styleSize);
				m_lastSnippetScrollBar->setStyleSheet(
					style +
					("\nQScrollBar:horizontal { background: " +
					 ColorScheme::getInstance()->getColor("code/snippet/background") + "; }")
						.c_str());
			}
		}
		else
		{
			m_lastSnippetScrollBar->hide();
		}
	}
}
