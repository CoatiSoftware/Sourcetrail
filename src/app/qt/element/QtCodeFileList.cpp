#include "qt/element/QtCodeFileList.h"

#include <QScrollBar>
#include <QVBoxLayout>

#include "utility/file/FileSystem.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"

QtCodeFileList::QtCodeFileList(QWidget* parent)
	: QScrollArea(parent)
	, m_focusedTokenId(0)
{
	m_frame = std::make_shared<QFrame>(this);
	m_frame->setObjectName("code_file_list");

	QVBoxLayout* layout = new QVBoxLayout(m_frame.get());
	layout->setSpacing(8);
	layout->setContentsMargins(8, 8, 8, 8);
	layout->setAlignment(Qt::AlignTop);
	m_frame->setLayout(layout);

	setWidgetResizable(true);
	setWidget(m_frame.get());
}

QtCodeFileList::~QtCodeFileList()
{
}

QSize QtCodeFileList::sizeHint() const
{
	return QSize(800, 800);
}

void QtCodeFileList::addCodeSnippet(
	uint startLineNumber,
	const std::string& title,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile
){
	FilePath filePath = locationFile->getFilePath();
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
		std::shared_ptr<QtCodeFile> filePtr = std::make_shared<QtCodeFile>(locationFile->getFilePath(), this);
		m_files.push_back(filePtr);

		file = filePtr.get();
		m_frame->layout()->addWidget(file);
	}

	file->addCodeSnippet(startLineNumber, title, code, locationFile);
}

void QtCodeFileList::clearCodeSnippets()
{
	m_files.clear();
	this->verticalScrollBar()->setValue(0);
}

Id QtCodeFileList::getFocusedTokenId() const
{
	return m_focusedTokenId;
}

const std::vector<Id>& QtCodeFileList::getActiveTokenIds() const
{
	return m_activeTokenIds;
}

void QtCodeFileList::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_activeTokenIds = activeTokenIds;
	updateFiles();
}

const std::vector<std::string>& QtCodeFileList::getErrorMessages() const
{
	return m_errorMessages;
}

void QtCodeFileList::setErrorMessages(const std::vector<std::string>& errorMessages)
{
	m_errorMessages = errorMessages;
	updateFiles();
}

void QtCodeFileList::updateFiles()
{
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		file->updateContent();
	}
}

void QtCodeFileList::focusToken(Id tokenId)
{
	m_focusedTokenId = tokenId;
	updateFiles();
}

void QtCodeFileList::defocusToken()
{
	m_focusedTokenId = 0;
	updateFiles();
}
