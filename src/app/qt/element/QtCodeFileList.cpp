#include "qt/element/QtCodeFileList.h"

#include <QScrollBar>
#include <QVBoxLayout>

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "utility/file/FileSystem.h"

QtCodeFileList::QtCodeFileList(QWidget* parent)
	: QScrollArea(parent)
	, m_showMaximizeButton(true)
{
	m_frame = std::make_shared<QFrame>(this);

	QVBoxLayout* layout = new QVBoxLayout(m_frame.get());
	layout->setSpacing(10);
	layout->setContentsMargins(15, 15, 15, 15);
	layout->setAlignment(Qt::AlignTop);
	m_frame->setLayout(layout);

	setWidgetResizable(true);
	setWidget(m_frame.get());

	setObjectName("code_file_list");
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
	const std::string& code,
	const TokenLocationFile& locationFile
){
	std::string fileName = FileSystem::fileName(locationFile.getFilePath());
	QtCodeFile* file = nullptr;

	for (std::shared_ptr<QtCodeFile> filePtr : m_files)
	{
		if (filePtr->getFileName() == fileName)
		{
			file = filePtr.get();
			break;
		}
	}

	if (!file)
	{
		std::shared_ptr<QtCodeFile> filePtr = std::make_shared<QtCodeFile>(locationFile.getFilePath(), this);
		m_files.push_back(filePtr);

		file = filePtr.get();
		m_frame->layout()->addWidget(file);
	}

	file->addCodeSnippet(startLineNumber, code, locationFile);
}

void QtCodeFileList::clearCodeSnippets()
{
	m_files.clear();
	this->verticalScrollBar()->setValue(0);
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

bool QtCodeFileList::getShowMaximizeButton() const
{
	return m_showMaximizeButton;
}

void QtCodeFileList::setShowMaximizeButton(bool show)
{
	m_showMaximizeButton = show;
	updateFiles();
}

void QtCodeFileList::updateFiles()
{
	for (std::shared_ptr<QtCodeFile> file: m_files)
	{
		file->update();
	}
}
