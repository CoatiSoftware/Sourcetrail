#ifndef STORAGE_CACHE_H
#define STORAGE_CACHE_H

#include <map>

#include "StorageAccessProxy.h"

class StorageCache
	: public StorageAccessProxy
{
public:
	void clear();

	std::shared_ptr<Graph> getGraphForAll() const override;

	StorageStats getStorageStats() const override;

	std::shared_ptr<TextAccess> getFileContent(const FilePath& filePath, bool showsErrors) const override;

	ErrorCountInfo getErrorCount() const override;
	std::vector<ErrorInfo> getErrorsLimited(const ErrorFilter& filter) const override;
	std::vector<ErrorInfo> getErrorsForFileLimited(const ErrorFilter& filter, const FilePath& filePath) const override;
	std::shared_ptr<SourceLocationCollection> getErrorSourceLocations(const std::vector<ErrorInfo>& errors) const override;

	void setUseErrorCache(bool enabled) override;
	void addErrorsToCache(const std::vector<ErrorInfo>& newErrors, const ErrorCountInfo& errorCount) override;

private:
	mutable std::shared_ptr<Graph> m_graphForAll;
	mutable StorageStats m_storageStats;

	bool m_useErrorCache = false;
	ErrorCountInfo m_errorCount;
	std::vector<ErrorInfo> m_cachedErrors;
};

#endif // STORAGE_CACHE_H
