#include "InterprocessProcessManager.h"

#include <thread>

#include "utility/logging/logging.h"

InterprocessProcessManager::InterprocessProcessManager()
	: m_processName("")
	, m_processCount(1)
{
}

InterprocessProcessManager::~InterprocessProcessManager()
{
}

void InterprocessProcessManager::setProcessName(const std::string& processName)
{
	m_processName = processName;
}

std::string InterprocessProcessManager::getProcessName() const
{
	return m_processName;
}

void InterprocessProcessManager::setProcessCount(const unsigned int processCount)
{
	m_processCount = processCount;
}

unsigned int InterprocessProcessManager::getProcessCount() const
{
	return m_processCount;
}

void InterprocessProcessManager::runProcesses()
{
	std::vector<std::thread*> processThreads;

	for (unsigned int i = 0; i < m_processCount; i++)
	{
		std::thread* thread = new std::thread(&InterprocessProcessManager::runProcess, this);
		processThreads.push_back(thread);
	}

	for (unsigned int i = 0; i < m_processCount; i++)
	{
		processThreads[i]->join();

		delete processThreads[i];
	}
}

void InterprocessProcessManager::runProcess()
{
	int result = 1;

	while (result != 0)
	{
		result = system(m_processName.c_str());

		LOG_INFO_STREAM(<< "Process returned with " << std::to_string(result));
	}
}
