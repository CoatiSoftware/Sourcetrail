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
	struct Arguments
	{
		Arguments();

		std::vector<std::string> headerSearchPaths;
		std::vector<std::string> systemHeaderSearchPaths;
		std::vector<std::string> frameworkSearchPaths;
		std::vector<std::string> compilerFlags;
		bool logErrors;
	};

	Parser(ParserClient* client);
	virtual ~Parser();

	virtual void parseFiles(const std::vector<FilePath>& filePaths, const Arguments& arguments) = 0;
	virtual void parseFile(std::shared_ptr<TextAccess> textAccess, const Arguments& arguments) = 0;

protected:
	ParserClient* m_client;
};

#endif // PARSER_H
