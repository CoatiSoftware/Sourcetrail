#ifndef INTERMEDIATE_STORAGE_H
#define INTERMEDIATE_STORAGE_H

#include <memory>
#include <map>
#include <unordered_map>

#include "data/StorageTypes.h"
#include "data/Storage.h"

class IntermediateStorage: public Storage
{
public:
	IntermediateStorage();
	virtual ~IntermediateStorage();

	virtual Id addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime);
	virtual Id addNode(int type, const std::string& serializedName, int definitionType);
	virtual Id addEdge(int type, Id sourceId, Id targetId);
	virtual Id addLocalSymbol(const std::string& name);
	virtual void addSourceLocation(Id elementId, Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);
	virtual void addComponentAccess(Id edgeId , int type);
	virtual void addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	virtual void addError(const std::string& message, bool fatal, bool indexed, const std::string& filePath, uint startLine, uint startCol);

	virtual void forEachFile(std::function<void(const Id /*id*/, const StorageFile& /*data*/)> callback) const;
	virtual void forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const;
	virtual void forEachEdge(std::function<void(const Id /*id*/, const StorageEdge& /*data*/)> callback) const;
	virtual void forEachLocalSymbol(std::function<void(const Id /*id*/, const StorageLocalSymbol& /*data*/)> callback) const;
	virtual void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const;
	virtual void forEachError(std::function<void(const StorageError& /*data*/)> callback) const;

private:
	std::string serialize(const StorageEdge& edge);
	std::string serialize(const StorageNode& node);
	std::string serialize(const StorageFile& file);
	std::string serialize(const StorageLocalSymbol& localSymbol);

	std::unordered_map<std::string, Id> m_fileNamesToIds; // this is used to prevent duplicates (unique)
	std::unordered_map<Id, std::shared_ptr<StorageFile>> m_fileIdsToData;

	std::unordered_map<std::string, Id> m_nodeNamesToIds; // this is used to prevent duplicates (unique)
	std::map<Id, std::shared_ptr<StorageNode>> m_nodeIdsToData;

	std::unordered_map<std::string, Id> m_edgeNamesToIds; // this is used to prevent duplicates (unique)
	std::map<Id, std::shared_ptr<StorageEdge>> m_edgeIdsToData;


	std::unordered_map<std::string, Id> m_localSymbolNamesToIds; // this is used to prevent duplicates (unique)
	std::map<Id, std::shared_ptr<StorageLocalSymbol>> m_localSymbolIdsToData;

	std::vector<StorageSourceLocation> m_sourceLocations;
	std::vector<StorageComponentAccess> m_componentAccesses;
	std::vector<StorageCommentLocation> m_commentLocations;
	std::vector<StorageError> m_errors;

	std::unordered_map<Id, Id> m_nodeIdsToMemberEdgeIds;

	Id m_nextId;
};

#endif // INTERMEDIATE_STORAGE_H
