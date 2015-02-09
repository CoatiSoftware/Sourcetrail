#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "data/parser/Parser.h"
#include "utility/file/FileManager.h"

class CxxParser: public Parser
{
public:
	CxxParser(ParserClient* client, FileManager* fileManager);
	~CxxParser();

	virtual void parseFiles(
		const std::vector<std::string>& filePaths,
		const std::vector<std::string>& systemHeaderSearchPaths,
		const std::vector<std::string>& headerSearchPaths);
	virtual void parseFile(std::shared_ptr<TextAccess> textAccess);

private:
	FileManager* m_fileManager;
};

#endif // CXX_PARSER_H
