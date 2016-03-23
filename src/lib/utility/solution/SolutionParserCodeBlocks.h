#ifndef SOLUTION_PARSER_CODE_BLOCKS_H
#define SOLUTION_PARSER_CODE_BLOCKS_H

#include "ISolutionParser.h"

#include "tinyxml/tinyxml.h"

class SolutionParserCodeBlocks : public ISolutionParser
{
public:
	SolutionParserCodeBlocks();
	virtual ~SolutionParserCodeBlocks();

	virtual std::string getToolID() const;

	virtual std::string getSolutionName();

	virtual std::vector<std::string> getProjects();
	virtual std::vector<std::string> getProjectFiles();
	virtual std::vector<std::string> getProjectItems();
	virtual std::vector<std::string> getIncludePaths();

	virtual ProjectSettings getProjectSettings(const std::string& solutionFilePath);

	virtual std::string getIdeName() const;
	virtual std::string getDescription() const;
	virtual std::string getIconPath() const;
	virtual std::string getFileExtension() const;
};

#endif // SOLUTION_PARSER_CODE_BLOCKS_H
