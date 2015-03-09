#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>
#include <vector>

#include "data/parser/ParserClient.h"

class FilePath;
class TextAccess;

class Parser
{
public:
	Parser(ParserClient* client);
	virtual ~Parser();

	virtual void parseFiles(
		const std::vector<FilePath>& filePaths,
		const std::vector<std::string>& systemHeaderSearchPaths,
		const std::vector<std::string>& headerSearchPaths) = 0;
	virtual void parseFile(std::shared_ptr<TextAccess> textAccess) = 0;

protected:
	ParserClient* m_client;
};

#endif // PARSER_H
