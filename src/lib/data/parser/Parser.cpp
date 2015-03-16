#include "data/parser/Parser.h"

Parser::Arguments::Arguments()
	: logErrors(true)
{
}

Parser::Parser(ParserClient* client)
	: m_client(client)
{
}

Parser::~Parser()
{
}
