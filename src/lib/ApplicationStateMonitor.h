#ifndef APPLICATION_STATE_MONITOR_H
#define APPLICATION_STATE_MONITOR_H

#include <mutex>
#include <set>
#include <vector>

class FilePath;

class ApplicationStateMonitor
{
public:
	static std::shared_ptr<ApplicationStateMonitor> getInstance();
	static std::vector<FilePath> getStoredIndexingFiles();
	static void clearStoredIndexingFiles();

	void clearIndexingFiles();
	void addIndexingFile(FilePath file);
	void removeIndexingFile(FilePath file);

private:
	static std::shared_ptr<ApplicationStateMonitor> s_instance;
	ApplicationStateMonitor();
	void storeCurrentState();


	std::set<FilePath> m_indexingFiles;
	std::mutex m_indexingFilesMutex;
};

#endif // APPLICATION_STATE_MONITOR_H
