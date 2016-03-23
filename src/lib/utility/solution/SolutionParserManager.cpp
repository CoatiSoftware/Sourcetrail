#include "SolutionParserManager.h"

#include <boost/algorithm/string.hpp>

#include "utility/logging/logging.h"

SolutionParserManager::SolutionParserManager()
	: m_solutionParsers()
{

}

SolutionParserManager::~SolutionParserManager()
{

}

void SolutionParserManager::pushSolutionParser(const std::shared_ptr<ISolutionParser>& solutionParser)
{
	m_solutionParsers.push_back(solutionParser);
}

bool SolutionParserManager::canParseSolution(const std::string& ideId) const
{
	std::string lIdeId = ideId;
	boost::algorithm::to_lower(lIdeId);

	for (unsigned int i = 0; i < m_solutionParsers.size(); i++)
	{
		std::string parserIdeId = m_solutionParsers[i]->getToolID();
		boost::algorithm::to_lower(parserIdeId);

		if (lIdeId == parserIdeId)
		{
			return true;
		}
	}

	return false;
}

ProjectSettings SolutionParserManager::getProjectSettings(const std::string& ideId, const std::string& solutionFilePath) const
{
	std::string lIdeId = ideId;
	boost::algorithm::to_lower(lIdeId);

	for (unsigned int i = 0; i < m_solutionParsers.size(); i++)
	{
		std::string parserIdeId = m_solutionParsers[i]->getToolID();
		boost::algorithm::to_lower(parserIdeId);

		if (lIdeId == parserIdeId)
		{
			return m_solutionParsers[i]->getProjectSettings(solutionFilePath);
		}
	}

	LOG_ERROR_STREAM(<< "Solution type is unknown");

	return ProjectSettings();
}

unsigned int SolutionParserManager::getParserCount() const
{
	return m_solutionParsers.size();
}

std::string SolutionParserManager::getParserName(const unsigned int idx) const
{
	if (checkIndex(idx))
	{
		return m_solutionParsers[idx]->getIdeName();
	}

	LOG_WARNING_STREAM(<< "Index is out of range, was " << idx << ". Max is " << m_solutionParsers.size() - 1);

	return "";
}

std::string SolutionParserManager::getParserDescription(const unsigned int idx) const
{
	if (checkIndex(idx))
	{
		return m_solutionParsers[idx]->getDescription();
	}

	LOG_WARNING_STREAM(<< "Index is out of range, was " << idx << ". Max is " << m_solutionParsers.size() - 1);

	return "";
}

std::string SolutionParserManager::getParserFileEnding(const unsigned int idx) const
{
	if (checkIndex(idx))
	{
		return m_solutionParsers[idx]->getFileExtension();
	}

	LOG_WARNING_STREAM(<< "Index is out of range, was " << idx << ". Max is " << m_solutionParsers.size() - 1);

	return "";
}

std::string SolutionParserManager::getParserIdeId(const unsigned int idx) const
{
	if (checkIndex(idx))
	{
		return m_solutionParsers[idx]->getToolID();
	}

	LOG_WARNING_STREAM(<< "Index is out of range, was " << idx << ". Max is " << m_solutionParsers.size() - 1);

	return "";
}

bool SolutionParserManager::checkIndex(const unsigned int idx) const
{
	return (idx < m_solutionParsers.size());
}
