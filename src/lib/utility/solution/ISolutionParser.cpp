#include "ISolutionParser.h"

std::string ISolutionParser::getSolutionPath()
{
	return m_solutionPath;
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
