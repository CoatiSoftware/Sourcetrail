#include "data/parser/java/TaskParseJava.h"

#include "component/view/DialogView.h"
#include "data/parser/java/JavaParser.h"
#include "data/parser/ParserClientImpl.h"
#include "data/IntermediateStorage.h"
#include "utility/file/FileRegister.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"

TaskParseJava::TaskParseJava(
	std::shared_ptr<IntermediateStorage> storage,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
	: m_storage(storage)
	, m_fileRegister(fileRegister)
	, m_arguments(arguments)
	, m_dialogView(dialogView)
	, m_interrupted(false)
{
}

void TaskParseJava::doEnter()
{
}

Task::TaskState TaskParseJava::doUpdate()
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();
	std::shared_ptr<JavaParser> parser = std::make_shared<JavaParser>(parserClient.get());

	FilePath sourcePath = m_fileRegister->consumeSourceFile();

	if (!sourcePath.empty())
	{
		m_dialogView->updateIndexingDialog(
			m_fileRegister->getParsedSourceFilesCount(), m_fileRegister->getSourceFilesCount(), sourcePath.str()
		);

		m_storage->clear();
		parserClient->setStorage(m_storage);
		parserClient->startParsingFile();

		parser->parseFile(sourcePath, TextAccess::createFromFile(sourcePath.str()), m_arguments);

		parserClient->finishParsingFile();
		parserClient->resetStorage();

		m_fileRegister->markThreadFilesParsed(); // todo: rename to markThreadFilesProcessed
	}

	return (m_interrupted ? STATE_FAILURE : STATE_SUCCESS);
}

void TaskParseJava::doExit()
{
}

void TaskParseJava::doReset()
{
}

void TaskParseJava::handleMessage(MessageInterruptTasks* message)
{
	m_interrupted = true;
}
