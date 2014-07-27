#include "qt/element/QtCodeFileList.h"

#include <QVBoxLayout>

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "utility/FileSystem.h"

QtCodeFileList::QtCodeFileList(QWidget* parent)
	: QScrollArea(parent)
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

void QtCodeFileList::addCodeSnippet(
	int startLineNumber,
	const std::string& code,
	const TokenLocationFile& locationFile,
	const std::vector<Id>& activeTokenIds
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
		std::shared_ptr<QtCodeFile> filePtr = std::make_shared<QtCodeFile>(fileName, this);
		m_files.push_back(filePtr);

		file = filePtr.get();
		m_frame->layout()->addWidget(file);
	}

	file->addCodeSnippet(startLineNumber, code, locationFile, activeTokenIds);
}

void QtCodeFileList::clearCodeSnippets()
{
	m_files.clear();
}
