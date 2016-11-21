#include "data/parser/java/TaskParseJava.h"

#include "component/view/DialogView.h"
#include "data/parser/java/JavaParser.h"
#include "data/parser/ParserClientImpl.h"
#include "data/StorageProvider.h"
#include "utility/file/FileRegister.h"
#include "utility/text/TextAccess.h"

TaskParseJava::TaskParseJava(
	std::shared_ptr<StorageProvider> storageProvider,
	std::shared_ptr<FileRegister> fileRegister,
	const Parser::Arguments& arguments,
	DialogView* dialogView
)
	: TaskParse(storageProvider, fileRegister, arguments, dialogView)
{
}

void TaskParseJava::indexFile(FilePath sourcePath)
{
	std::shared_ptr<ParserClientImpl> parserClient = std::make_shared<ParserClientImpl>();
	std::shared_ptr<JavaParser> parser = std::make_shared<JavaParser>(parserClient.get());

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
