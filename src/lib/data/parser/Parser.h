#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <string>
#include <vector>

#include "utility/file/FilePath.h"

class ParserClient;
class TextAccess;

class Parser
{
public:
	struct Arguments
	{
		Arguments();

		std::vector<FilePath> javaClassPaths;

		std::vector<FilePath> headerSearchPaths;
		std::vector<FilePath> systemHeaderSearchPaths;
		std::vector<FilePath> frameworkSearchPaths;
		std::vector<std::string> compilerFlags;

		bool logErrors;

		std::string language;
		std::string languageStandard;

		FilePath compilationDatabasePath;
	};

	Parser(ParserClient* client);
	virtual ~Parser();

	virtual void parseFiles(const std::vector<FilePath>& filePaths, const Arguments& arguments) = 0;
	virtual void parseFile(const FilePath& filePath, std::shared_ptr<TextAccess> textAccess, const Arguments& arguments) = 0;

protected:
	ParserClient* m_client;
};

#endif // PARSER_H
