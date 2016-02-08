#ifndef SOLUTION_PARSER_C_MAKE_H
#define SOLUTION_PARSER_C_MAKE_H

#include "ISolutionParser.h"

class SolutionParserCMake : public ISolutionParser
{
public:
	SolutionParserCMake();
	virtual ~SolutionParserCMake();

	virtual std::vector<std::string> getProjects();
	virtual std::vector<std::string> getProjectFiles();
};

#endif // SOLUTION_PARSER_C_MAKE_H