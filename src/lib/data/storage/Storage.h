#ifndef STORAGE_H
#define STORAGE_H

#include <functional>
#include <mutex>
#include <string>

#include "StorageCommentLocation.h"
#include "StorageComponentAccess.h"
#include "StorageEdge.h"
#include "StorageError.h"
#include "StorageFile.h"
#include "StorageLocalSymbol.h"
#include "StorageNode.h"
#include "StorageOccurrence.h"
#include "StorageSourceLocation.h"
#include "StorageSymbol.h"
#include "types.h"

class Storage
{
public:
	Storage();
	virtual ~Storage() = default;

	virtual std::pair<Id, bool> addNode(const StorageNodeData& data) = 0;
	virtual void addSymbol(const StorageSymbol& data) = 0;
	virtual void addFile(const StorageFile& data) = 0;
	virtual Id addEdge(const StorageEdgeData& data) = 0;
	virtual Id addLocalSymbol(const StorageLocalSymbolData& data) = 0;
	virtual Id addSourceLocation(const StorageSourceLocationData& data) = 0;
	virtual void addOccurrence(const StorageOccurrence& data) = 0;
	virtual void addOccurrences(const std::vector<StorageOccurrence>& occurrences) = 0;
	virtual void addComponentAccess(const StorageComponentAccess& componentAccess) = 0;
	virtual void addCommentLocation(const StorageCommentLocationData& data) = 0;
	virtual void addError(const StorageErrorData& data) = 0;

	virtual void forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const = 0;
	virtual void forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const = 0;
	virtual void forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const = 0;
	virtual void forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const = 0;
	virtual void forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const = 0;
	virtual void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const = 0;
	virtual void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const = 0;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const = 0;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocationData& /*data*/)> callback) const = 0;
	virtual void forEachError(std::function<void(const StorageErrorData& /*data*/)> callback) const = 0;

	void inject(Storage* injected);

private:
	virtual void startInjection();
	virtual void finishInjection();

	std::mutex m_dataMutex;
};

#endif // STORAGE_H
