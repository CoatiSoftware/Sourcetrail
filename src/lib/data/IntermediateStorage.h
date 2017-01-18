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

	void clear();
	size_t getSourceLocationCount() const;

	virtual Id addNode(int type, const std::string& serializedName);
	virtual void addFile(const Id id, const std::string& filePath, const std::string& modificationTime);
	virtual void addSymbol(const Id id, int definitionType);
	virtual Id addEdge(int type, Id sourceId, Id targetId);
	virtual Id addLocalSymbol(const std::string& name);
	virtual Id addSourceLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);
	virtual void addOccurrence(Id elementId, Id sourceLocationId);
	virtual void addComponentAccess(Id nodeId , int type);
	virtual void addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	virtual void addError(const std::string& message, const FilePath& filePath, uint startLine, uint startCol, bool fatal, bool indexed);

	virtual void forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const;
	virtual void forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const;
	virtual void forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const;
	virtual void forEachEdge(std::function<void(const Id /*id*/, const StorageEdge& /*data*/)> callback) const;
	virtual void forEachLocalSymbol(std::function<void(const Id /*id*/, const StorageLocalSymbol& /*data*/)> callback) const;
	virtual void forEachSourceLocation(std::function<void(const Id /*id*/, const StorageSourceLocation& /*data*/)> callback) const;
	virtual void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const;
	virtual void forEachError(std::function<void(const StorageError& /*data*/)> callback) const;

private:
	std::string serialize(const StorageEdge& edge) const;
	std::string serialize(const StorageNode& node) const;
	std::string serialize(const StorageLocalSymbol& localSymbol) const;
	std::string serialize(const StorageSourceLocation& sourceLocation) const;

	std::unordered_map<std::string, Id> m_nodeNamesToIds; // this is used to prevent duplicates (unique)
	std::map<Id, std::shared_ptr<StorageNode>> m_nodeIdsToData;

	std::vector<StorageFile> m_files;
	std::vector<StorageSymbol> m_symbols;

	std::unordered_map<std::string, Id> m_edgeNamesToIds; // this is used to prevent duplicates (unique)
	std::map<Id, std::shared_ptr<StorageEdge>> m_edgeIdsToData;


	std::unordered_map<std::string, Id> m_localSymbolNamesToIds; // this is used to prevent duplicates (unique)
	std::map<Id, std::shared_ptr<StorageLocalSymbol>> m_localSymbolIdsToData;

	std::unordered_map<std::string, Id> m_sourceLocationNamesToIds; // this is used to prevent duplicates (unique)
	std::map<Id, std::shared_ptr<StorageSourceLocation>> m_sourceLocationIdsToData;

	std::vector<StorageOccurrence> m_occurrences;
	std::vector<StorageComponentAccess> m_componentAccesses;
	std::vector<StorageCommentLocation> m_commentLocations;
	std::vector<StorageError> m_errors;

	Id m_nextId;
};

#endif // INTERMEDIATE_STORAGE_H
