#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "data/parser/Parser.h"

class CxxParser: public Parser
{
public:
	CxxParser(ParserClient* client);
	~CxxParser();

	virtual void parseFiles(
		const std::vector<std::string>& filePaths,
		const std::vector<std::string>& systemHeaderSearchPaths,
		const std::vector<std::string>& headerSearchPaths);
	virtual void parseFile(std::shared_ptr<TextAccess> textAccess);
};

#endif // CXX_PARSER_H
