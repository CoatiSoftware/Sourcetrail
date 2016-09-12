#include "data/parser/java/TaskParseJava.h"

#include "component/view/DialogView.h"
#include "data/parser/java/JavaParser.h"
#include "data/parser/ParserClientImpl.h"
#include "data/StorageProvider.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/scheduling/Blackboard.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"

TaskParseJava::TaskParseJava(
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
	: m_storageProvider(storageProvider)
	, m_fileRegister(fileRegister)
	, m_arguments(arguments)
	, m_dialogView(dialogView)
	, m_interrupted(false)
{
}

void TaskParseJava::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	int indexerCount = 0;
	if (blackboard->get("indexer_count", indexerCount))
	{
		indexerCount++;
		blackboard->set("indexer_count", indexerCount);
	}
}

Task::TaskState TaskParseJava::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();
	std::shared_ptr<JavaParser> parser = std::make_shared<JavaParser>(parserClient.get());

	FilePath sourcePath = m_fileRegister->consumeSourceFile();

	if (sourcePath.empty())
	{
		return STATE_FAILURE;
	}
	else
	{
		m_dialogView->updateIndexingDialog(
			m_fileRegister->getParsedSourceFilesCount(), m_fileRegister->getSourceFilesCount(), sourcePath.str()
		);

		std::shared_ptr<IntermediateStorage> storage = m_storageProvider->popIndexerTarget();
		parserClient->setStorage(storage);
		parserClient->startParsingFile();

		parser->parseFile(sourcePath, TextAccess::createFromFile(sourcePath.str()), m_arguments);

		parserClient->finishParsingFile();
		parserClient->resetStorage();

		if (!m_interrupted)
		{
			m_fileRegister->markThreadFilesParsed(); // todo: rename to markThreadFilesProcessed
			m_storageProvider->pushIndexerTarget(storage);
		}
	}

	return (m_interrupted ? STATE_FAILURE : STATE_SUCCESS);
}

void TaskParseJava::doExit(std::shared_ptr<Blackboard> blackboard)
{
	int indexerCount = 0;
	if (blackboard->get("indexer_count", indexerCount))
	{
		indexerCount--;
		blackboard->set("indexer_count", indexerCount);
	}
}

void TaskParseJava::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskParseJava::handleMessage(MessageInterruptTasks* message)
{
	m_interrupted = true;
}
