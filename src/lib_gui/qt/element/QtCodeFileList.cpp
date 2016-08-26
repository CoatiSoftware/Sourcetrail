#include "qt/element/QtCodeFileList.h"

#include <QVBoxLayout>

#include "utility/file/FileSystem.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFileList::QtCodeFileList(QtCodeNavigator* navigator)
	: QFrame()
	, m_navigator(navigator)
{
	setObjectName("code_file_list");

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(8);
	layout->setContentsMargins(8, 8, 8, 8);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);
}

QtCodeFileList::~QtCodeFileList()
{
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

void QtCodeFileList::addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount, TimePoint modificationTime)
{
	QtCodeFile* file = getFile(locationFile->getFilePath());
	file->setLocationFile(locationFile, refCount);
	file->setModificationTime(modificationTime);
}

void QtCodeFileList::clearCodeSnippets()
{
	m_files.clear();
}

void QtCodeFileList::requestFileContent(const FilePath& filePath)
{
	getFile(filePath)->requestContent();
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
		layout()->addWidget(file);

		file->hide();
	}

	return file;
}
