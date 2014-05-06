#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "data/parser/Parser.h"

class CxxParser: public Parser
{
public:
	CxxParser(std::shared_ptr<ParserClient> client);
	~CxxParser();

	virtual void parseFiles(const std::vector<std::string>& filePaths);
};

#endif // CXX_PARSER_H
