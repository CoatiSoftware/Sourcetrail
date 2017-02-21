#ifndef INTERPROCESS_PROCESS_MANAGER_H
#define INTERPROCESS_PROCESS_MANAGER_H

#include <string>

class InterprocessProcessManager
{
public:
	InterprocessProcessManager();
	~InterprocessProcessManager();

	void setProcessName(const std::string& processName);
	std::string getProcessName() const;

	void setProcessCount(const unsigned int processCount);
	unsigned int getProcessCount() const;

	void runProcesses();

private:
	void runProcess();

	std::string m_processName;
	unsigned int m_processCount;
};

#endif // INTERPROCESS_PROCESS_MANAGER_H
