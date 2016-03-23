#ifndef SOLUTION_PARSER_MANAGER_H
#define SOLUTION_PARSER_MANAGER_H

#include <vector>

#include "ISolutionParser.h"

class SolutionParserManager
{
public:
	SolutionParserManager();
	~SolutionParserManager();

	void pushSolutionParser(const std::shared_ptr<ISolutionParser>& solutionParser);

	bool canParseSolution(const std::string& ideId) const;

	ProjectSettings getProjectSettings(const std::string& ideId, const std::string& solutionFilePath) const;

	unsigned int getParserCount() const;

	std::string getParserName(const unsigned int idx) const;
	std::string getParserDescription(const unsigned int idx) const;
	std::string getParserFileEnding(const unsigned int idx) const;
	std::string getParserIdeId(const unsigned int idx) const;

private:
	bool checkIndex(const unsigned int idx) const;

	std::vector<std::shared_ptr<ISolutionParser>> m_solutionParsers;
};

#endif // SOLUTION_PARSER_MANAGER_H
