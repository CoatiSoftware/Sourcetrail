#include "qt/element/QtCodeFileList.h"

#include <QScrollBar>
#include <QVBoxLayout>

#include "utility/file/FileSystem.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFileList::QtCodeFileList(QtCodeNavigator* navigator)
	: QScrollArea()
	, m_navigator(navigator)
{
	setObjectName("code_container");
	setWidgetResizable(true);

	m_filesArea = new QFrame();
	m_filesArea->setObjectName("code_file_list");

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(8);
	layout->setContentsMargins(8, 8, 8, 8);
	layout->setAlignment(Qt::AlignTop);
	m_filesArea->setLayout(layout);

	setWidget(m_filesArea);

	m_scrollSpeedChangeListener.setScrollBar(verticalScrollBar());

	connect(verticalScrollBar(), SIGNAL(valueChanged(int)), m_navigator, SLOT(scrolled(int)));
}

QtCodeFileList::~QtCodeFileList()
{
}

void QtCodeFileList::clear()
{
	m_files.clear();
	verticalScrollBar()->setValue(0);
}

QtCodeFile* QtCodeFileList::getFile(const FilePath filePath)
{
	QtCodeFile* file = nullptr;

	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		if (filePtr->getFilePath() == filePath)
		{
			file = filePtr.get();
			break;
		}
	}

	if (!file)
	{
		std::shared_ptr<QtCodeFile> filePtr = std::make_shared<QtCodeFile>(filePath, m_navigator);
		m_files.push_back(filePtr);

		file = filePtr.get();
		m_filesArea->layout()->addWidget(file);

		file->hide();
	}

	return file;
}

void QtCodeFileList::addFile(const FilePath& filePath, bool isWholeFile, int refCount, TimePoint modificationTime)
{
	QtCodeFile* file = getFile(filePath);
	file->setWholeFile(isWholeFile, refCount);
	file->setModificationTime(modificationTime);
}

QScrollArea* QtCodeFileList::getScrollArea()
{
	return this;
}

void QtCodeFileList::addCodeSnippet(
	const CodeSnippetParams& params,
	bool insert
){
	QtCodeFile* file = getFile(params.locationFile->getFilePath());
	QtCodeSnippet* snippet = nullptr;

	if (insert)
	{
		snippet = file->insertCodeSnippet(params);
	}
	else
	{
		snippet = file->addCodeSnippet(params);
	}

	file->setModificationTime(params.modificationTime);
}

void QtCodeFileList::requestFileContent(const FilePath& filePath)
{
	getFile(filePath)->requestContent();
}

bool QtCodeFileList::requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, bool onTop)
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
		snippet = file->getFileSnippet();
	}

	if (!snippet)
	{
		return true;
	}

	if (!snippet->isVisible())
	{
		file->setSnippets();
	}

	if (!lineNumber)
	{
		if (locationId)
		{
			lineNumber = snippet->getLineNumberForLocationId(locationId);
		}
		else
		{
			lineNumber = 1;
		}
	}

	ensureWidgetVisibleAnimated(m_filesArea, snippet, snippet->getLineRectForLineNumber(lineNumber), animated, onTop);

	return true;
}

void QtCodeFileList::updateFiles()
{
	for (std::shared_ptr<QtCodeFile> file : m_files)
	{
		file->updateContent();
	}
}

void QtCodeFileList::showContents()
{
	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		filePtr->show();
	}
}

void QtCodeFileList::onWindowFocus()
{
	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		filePtr->updateTitleBar();
	}
}

void QtCodeFileList::setFileMinimized(const FilePath path)
{
	getFile(path)->setMinimized();
}

void QtCodeFileList::setFileSnippets(const FilePath path)
{
	getFile(path)->setSnippets();
}

void QtCodeFileList::setFileMaximized(const FilePath path)
{
	getFile(path)->setMaximized();
}

std::pair<QtCodeSnippet*, uint> QtCodeFileList::getFirstSnippetWithActiveLocation(Id tokenId) const
{
	std::pair<QtCodeSnippet*, uint> result(nullptr, 0);

	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		if (filePtr->isCollapsed())
		{
			continue;
		}

		result = filePtr->getFirstSnippetWithActiveLocation(tokenId);
		if (result.first != nullptr)
		{
			break;
		}
	}

	return result;
}
