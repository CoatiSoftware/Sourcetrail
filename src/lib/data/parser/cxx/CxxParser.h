#ifndef CXX_PARSER_H
#define CXX_PARSER_H

#include "data/parser/Parser.h"
#include "utility/file/FileManager.h"

class CxxParser: public Parser
{
public:
	CxxParser(ParserClient* client, const FileManager* fileManager);
	~CxxParser();

	virtual void parseFiles(const std::vector<FilePath>& filePaths, const Arguments& arguments);
	virtual void parseFile(std::shared_ptr<TextAccess> textAccess, const Arguments& arguments);

private:
	std::vector<std::string> getCommandlineArguments(const Arguments& arguments) const;

	const FileManager* m_fileManager;
};

#endif // CXX_PARSER_H
