#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>
#include <vector>

class ParserClient;
class TextAccess;

class Parser
{
public:
	Parser(std::shared_ptr<ParserClient> client);
	virtual ~Parser();

protected:
	std::shared_ptr<ParserClient> m_client;
};

#endif // PARSER_H
