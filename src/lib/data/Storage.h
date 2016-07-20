#ifndef STORAGE_H
#define STORAGE_H

#include <functional>
#include <string>

#include "data/name/NameHierarchy.h"
#include "data/StorageTypes.h"
#include "utility/types.h"

class Storage
{
public:
	Storage();
	virtual ~Storage();

	virtual Id addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime) = 0;
	virtual Id addNode(int type, const std::string& serializedName, int definitionType) = 0;
	virtual Id addEdge(int type, Id sourceId, Id targetId) = 0;
	virtual Id addLocalSymbol(const std::string& name) = 0;
	virtual void addSourceLocation(Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type) = 0;
	virtual void addComponentAccess(Id edgeId , int type) = 0;
	virtual void addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol) = 0;
	virtual void addError(const std::string& message, bool fatal, bool indexed, const std::string& filePath, uint startLine, uint startCol) = 0;

	virtual void forEachFile(std::function<void(const Id /*id*/, const StorageFile& /*data*/)> callback) const = 0;
	virtual void forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const = 0;
	virtual void forEachEdge(std::function<void(const Id /*id*/, const StorageEdge& /*data*/)> callback) const = 0;
	virtual void forEachLocalSymbol(std::function<void(const Id /*id*/, const StorageLocalSymbol& /*data*/)> callback) const = 0;
	virtual void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const = 0;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const = 0;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const = 0;
	virtual void forEachError(std::function<void(const StorageError& /*data*/)> callback) const = 0;

	virtual void startInjection();
	virtual void finishInjection();
	void inject(Storage* injected);
};

#endif // STORAGE_H
