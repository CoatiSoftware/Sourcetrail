#include "ISolutionParser.h"

std::string ISolutionParser::getSolutionPath()
{
	return m_solutionPath;
}

std::string ISolutionParser::getToolID() const
{
	return "NONE";
}

void ISolutionParser::openSolutionFile(const std::string& solutionFilePath)
{
	m_solution = loadFile(solutionFilePath);

	size_t pos = solutionFilePath.find_last_of("/");
	if (pos == std::string::npos)
	{
		pos = solutionFilePath.find_last_of("\\");
	}

	if (pos != std::string::npos)
	{
		if (pos > 0)
		{
			pos += 1;
		}

		m_solutionPath = solutionFilePath.substr(0, pos);
		m_solutionName = solutionFilePath.substr(pos);
	}
}

unsigned int ISolutionParser::getSolutionCharCount() const
{
	return m_solution.size();
}

std::string ISolutionParser::loadFile(const std::string& filePath)
{
	std::string file = "";

	std::ifstream ifstream;
	ifstream.open(filePath);

	if (ifstream.is_open())
	{
		file = std::string(std::istreambuf_iterator<char>(ifstream), std::istreambuf_iterator<char>());
	}

	return file;
}

std::vector<std::string> ISolutionParser::makePathsAbsolute(const std::vector<std::string>& paths)
{
	std::vector<std::string> absolutePaths;

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		std::string path = paths[i];

		boost::filesystem::path boostPath(path);
		if (boostPath.is_relative())
		{
			path = m_solutionPath + path;
		}

		absolutePaths.push_back(path);
	}

	return absolutePaths;
}
