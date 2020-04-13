#include "IndexerCxx.h"

#include "../parser/cxx/CxxParser.h"
#include "../../../lib/utility/file/FileRegister.h"

void IndexerCxx::doIndex(
	std::shared_ptr<IndexerCommandCxx> indexerCommand,
	std::shared_ptr<ParserClientImpl> parserClient,
	std::shared_ptr<IndexerStateInfo> m_indexerStateInfo)
{
	CxxParser parser(
		parserClient,
		std::make_shared<FileRegister>(
			indexerCommand->getSourceFilePath(),
			indexerCommand->getIndexedPaths(),
			indexerCommand->getExcludeFilters()),
		m_indexerStateInfo);

	parser.buildIndex(indexerCommand);
}
