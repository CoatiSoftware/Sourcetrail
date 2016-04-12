#ifndef INTERMEDIATE_STORAGE_H
#define INTERMEDIATE_STORAGE_H

#include <memory>
#include <string>
#include <unordered_map>

#include "utility/types.h"
#include "data/name/NameHierarchy.h"
#include "data/parser/ParseLocation.h"

#include "data/SqliteStorage.h"
#include "data/StorageTypes.h"

class IntermediateStorage
{
public:
	IntermediateStorage();
	~IntermediateStorage();
	Id addEdge(int type, Id sourceId, Id targetId);
	Id addNode(int type, const NameHierarchy& nameHierarchy, int definitionType);
	Id addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime);
	Id addFile(const std::string& filePath);
	Id addLocalSymbol(const std::string& name);
	void addSourceLocation(Id elementId, const ParseLocation& location, int type);
	void addComponentAccess(Id nodeId , int type);
	void addCommentLocation(const ParseLocation& location);
	void addError(const std::string& message, bool fatal, const ParseLocation& location);

	void transferToStorage(SqliteStorage& storage); // TODO: remove this and use foreach-callbacks instead

	void forEachFile(std::function<void(const Id /*id*/, const StorageFile& /*data*/)> callback) const;
	void forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const;
	void forEachEdge(std::function<void(const Id /*id*/, const StorageEdge& /*data*/)> callback) const;
	void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const;
	void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const;
	void forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const;
	void forEachError(std::function<void(const StorageError& /*data*/)> callback) const;

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
