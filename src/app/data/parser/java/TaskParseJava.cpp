#include "data/parser/java/TaskParseJava.h"

#include "data/parser/java/JavaParser.h"
#include "data/parser/ParserClientImpl.h"
#include "data/PersistentStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"

TaskParseJava::TaskParseJava(
	PersistentStorage* storage,
	std::shared_ptr<std::mutex> storageMutex,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments
)
	: m_storage(storage)
	, m_storageMutex(storageMutex)
	, m_fileRegister(fileRegister)
	, m_arguments(arguments)
{
}

void TaskParseJava::enter()
{
}

Task::TaskState TaskParseJava::update()
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();
	std::shared_ptr<JavaParser> parser = std::make_shared<JavaParser>(parserClient.get());

	FilePath sourcePath = m_fileRegister->consumeSourceFile();

	if (sourcePath.empty())
	{
		return Task::STATE_FINISHED;
	}

	std::stringstream ss;
	ss << "indexing files (ESC to quit): [";
	ss << m_fileRegister->getParsedSourceFilesCount() << "/";
	ss << m_fileRegister->getSourceFilesCount() << "] ";
	ss << sourcePath.str();
	MessageStatus(ss.str(), false, true).dispatch();

	std::shared_ptr<IntermediateStorage> intermediateStorage = std::make_shared<IntermediateStorage>();

	parserClient->setStorage(intermediateStorage);
	parserClient->startParsingFile();

	parser->parseFile(sourcePath, TextAccess::createFromFile(sourcePath.str()), m_arguments);

	parserClient->finishParsingFile();
	parserClient->resetStorage();

	{
		std::lock_guard<std::mutex> lock(*(m_storageMutex.get()));
		m_storage->inject(intermediateStorage.get());
	}

	m_fileRegister->markThreadFilesParsed(); // todo: rename to markThreadFilesProcessed

	return Task::STATE_RUNNING;
}

void TaskParseJava::exit()
{
}

void TaskParseJava::interrupt()
{
}

void TaskParseJava::revert()
{
}
