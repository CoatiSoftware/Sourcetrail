#ifndef PARSER_CLIENT_H
#define PARSER_CLIENT_H

#include <string>

#include "data/parser/ParseObject.h"

class ParserClient
{
public:
	ParserClient();
	virtual ~ParserClient();

	virtual void addClass(const ParseObject& object) = 0;
};

#endif // PARSER_CLIENT_H
