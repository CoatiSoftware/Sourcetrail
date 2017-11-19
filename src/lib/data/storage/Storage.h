#ifndef STORAGE_H
#define STORAGE_H

#include <functional>
#include <mutex>
#include <string>

#include "data/storage/type/StorageCommentLocation.h"
#include "data/storage/type/StorageComponentAccess.h"
#include "data/storage/type/StorageEdge.h"
#include "data/storage/type/StorageError.h"
#include "data/storage/type/StorageFile.h"
#include "data/storage/type/StorageLocalSymbol.h"
#include "data/storage/type/StorageNode.h"
#include "data/storage/type/StorageOccurrence.h"
#include "data/storage/type/StorageSourceLocation.h"
#include "data/storage/type/StorageSymbol.h"
#include "utility/types.h"

class Storage
{
public:
	Storage();
	virtual ~Storage();

	virtual Id addNode(const StorageNodeData& data) = 0;
	virtual void addSymbol(const StorageSymbol& data) = 0;
	virtual void addFile(const StorageFile& data) = 0;
	virtual Id addEdge(const StorageEdgeData& data) = 0;
	virtual Id addLocalSymbol(const StorageLocalSymbolData& data) = 0;
	virtual Id addSourceLocation(const StorageSourceLocationData& data) = 0;
	virtual void addOccurrence(const StorageOccurrence& data) = 0;
	virtual void addComponentAccess(const StorageComponentAccessData& data) = 0;
	virtual void addCommentLocation(const StorageCommentLocationData& data) = 0;
	virtual void addError(const StorageErrorData& data) = 0;

	virtual void forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const = 0;
	virtual void forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const = 0;
	virtual void forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const = 0;
	virtual void forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const = 0;
	virtual void forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const = 0;
	virtual void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const = 0;
	virtual void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const = 0;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccessData& /*data*/)> callback) const = 0;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocationData& /*data*/)> callback) const = 0;
	virtual void forEachError(std::function<void(const StorageErrorData& /*data*/)> callback) const = 0;

	void inject(Storage* injected);

private:
	virtual void startInjection();
	virtual void finishInjection();

	std::mutex m_dataMutex;
};

#endif // STORAGE_H
