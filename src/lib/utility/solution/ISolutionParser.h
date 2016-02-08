#ifndef I_SOLUTION_PARSER_H
#define I_SOLUTION_PARSER_H

#include <fstream>
#include <vector>

class ISolutionParser
{
public:
	ISolutionParser()
		: m_solutionPath("")
		, m_solution("")
	{};
	virtual ~ISolutionParser(){};

	void openSolutionFile(const std::string& solutionFilePath);
	unsigned int getSolutionCharCount() const;

	virtual std::string getSolutionName() = 0; // to be overwritten to account for different file endings
	std::string getSolutionPath();

	virtual std::vector<std::string> getProjects() = 0;
	virtual std::vector<std::string> getProjectFiles() = 0;
	virtual std::vector<std::string> getProjectItems() = 0;

protected:
	std::string loadFile(const std::string& filePath);

	std::string m_solutionName;
	std::string m_solutionPath;
	std::string m_solution;
};

#endif // I_SOLUTION_PARSER_H
