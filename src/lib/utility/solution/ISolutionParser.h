#ifndef I_SOLUTION_PARSER_H
#define I_SOLUTION_PARSER_H

#include <fstream>
#include <vector>

#include "settings/ProjectSettings.h"

class ISolutionParser
{
public:
	ISolutionParser()
		: m_solutionPath("")
		, m_solution("")
	{};
	virtual ~ISolutionParser(){};

	virtual std::string getToolID() const;

	void openSolutionFile(const std::string& solutionFilePath);
	unsigned int getSolutionCharCount() const;

	virtual std::string getSolutionName() = 0; // to be overwritten to account for different file endings
	std::string getSolutionPath();

	virtual std::vector<std::string> getProjects() = 0;
	virtual std::vector<std::string> getProjectFiles() = 0;
	virtual std::vector<std::string> getProjectItems() = 0;
	virtual std::vector<std::string> getCompileFlags() = 0;

	virtual std::shared_ptr<ProjectSettings> getProjectSettings(const std::string& solutionFilePath) = 0;

	virtual std::string getIdeName() const = 0;
	virtual std::string getButtonText() const = 0;
	virtual std::string getDescription() const = 0;
	virtual std::string getIconPath() const = 0;
	virtual std::string getFileExtension() const = 0;

protected:
	std::string loadFile(const std::string& filePath);

	std::vector<std::string> makePathsAbsolute(const std::vector<std::string>& paths);

	std::string m_solutionName;
	std::string m_solutionPath;
	std::string m_solution;
};

#endif // I_SOLUTION_PARSER_H
