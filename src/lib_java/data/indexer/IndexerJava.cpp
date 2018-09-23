#include "IndexerJava.h"

#include "JavaParser.h"

IndexerJava::~IndexerJava()
{
	JavaParser::clearCaches();
}

void IndexerJava::doIndex(
		std::shared_ptr<IndexerCommandJava> indexerCommand,
		std::shared_ptr<ParserClientImpl> parserClient,
		std::shared_ptr<IndexerStateInfo> m_indexerStateInfo
){
	JavaParser(parserClient, m_indexerStateInfo).buildIndex(indexerCommand);
}
