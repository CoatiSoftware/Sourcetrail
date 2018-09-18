#include "Parser.h"

Parser::Parser(std::shared_ptr<ParserClient> client)
	: m_client(client)
{
}
