#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>
#include <vector>

#include "data/parser/ParserClient.h"

class Parser
{
public:
	Parser(std::shared_ptr<ParserClient> client);
	virtual ~Parser();

	virtual void parseFiles(const std::vector<std::string>& filePaths) = 0;

protected:
	std::shared_ptr<ParserClient> m_client;
};

#endif // PARSER_H
