#ifndef INTERMEDIATE_STORAGE_H
#define INTERMEDIATE_STORAGE_H

#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "data/storage/StorageTypes.h"
#include "data/storage/Storage.h"

class IntermediateStorage: public Storage
{
public:
	IntermediateStorage();
	virtual ~IntermediateStorage();

	void clear();

	size_t getByteSize(size_t stringSize) const;
	size_t getSourceLocationCount() const;

	void setAllFilesIncomplete();
	void setFilesWithErrorsIncomplete();

	virtual Id addNode(int type, const std::string& serializedName);
	virtual void addFile(const Id id, const std::string& filePath, const std::string& modificationTime, bool complete);
	virtual void addSymbol(const Id id, int definitionKind);
	virtual Id addEdge(int type, Id sourceId, Id targetId);
	virtual Id addLocalSymbol(const std::string& name);
	virtual Id addSourceLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol, int type);
	virtual void addOccurrence(Id elementId, Id sourceLocationId);
	virtual void addComponentAccess(Id nodeId , int type);
	virtual void addCommentLocation(Id fileNodeId, uint startLine, uint startCol, uint endLine, uint endCol);
	virtual void addError(const std::string& message, const std::string& commandline, const FilePath& filePath,
		uint startLine, uint startCol, bool fatal, bool indexed);

	virtual void forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const;
	virtual void forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const;
	virtual void forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const;
	virtual void forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const;
	virtual void forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const;
	virtual void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const;
	virtual void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const;
	virtual void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const;
	virtual void forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const;
	virtual void forEachError(std::function<void(const StorageError& /*data*/)> callback) const;


	// for conversion to and from 'SharedIntermediateStorage'

	std::vector<StorageNode> getStorageNodes() const;
	std::vector<StorageFile> getStorageFiles() const;
	std::vector<StorageSymbol> getStorageSymbols() const;
	std::vector<StorageEdge> getStorageEdges() const;
	std::vector<StorageLocalSymbol> getStorageLocalSymbols() const;
	std::vector<StorageSourceLocation> getStorageSourceLocations() const;
	std::vector<StorageOccurrence> getStorageOccurrences() const;
	std::vector<StorageComponentAccess> getComponentAccesses() const;
	std::vector<StorageCommentLocation> getCommentLocations() const;
	std::vector<StorageError> getErrors() const;

	void setStorageNodes(const std::vector<StorageNode>& storageNodes);
	void setStorageFiles(const std::vector<StorageFile>& storageFiles);
	void setStorageSymbols(const std::vector<StorageSymbol>& storageSymbols);
	void setStorageEdges(const std::vector<StorageEdge>& storageEdges);
	void setStorageLocalSymbols(const std::vector<StorageLocalSymbol>& storageLocalSymbols);
	void setStorageSourceLocations(const std::vector<StorageSourceLocation>& storageSourceLocations);
	void setStorageOccurrences(const std::vector<StorageOccurrence>& storageOccurrences);
	void setComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses);
	void setCommentLocations(const std::vector<StorageCommentLocation>& commentLocations);
	void setErrors(const std::vector<StorageError>& errors);

	Id getNextId() const;
	void setNextId(const Id nextId);

private:
	std::string serialize(const StorageNode& node) const;
	std::string serialize(const StorageFile& file) const;
	std::string serialize(const StorageEdge& edge) const;
	std::string serialize(const StorageLocalSymbol& localSymbol) const;
	std::string serialize(const StorageSourceLocation& sourceLocation) const;
	std::string serialize(const StorageOccurrence& occurrence) const;
	std::string serialize(const StorageComponentAccess& componentAccess) const;
	std::string serialize(const StorageCommentLocation& commentLocation) const;
	std::string serialize(const StorageError& error) const;

	std::unordered_map<std::string, size_t> m_nodesIndex;
	std::vector<StorageNode> m_nodes;

	std::unordered_set<std::string> m_serializedFiles; // this is used to prevent duplicates (unique)
	std::vector<StorageFile> m_files;

	std::vector<StorageSymbol> m_symbols;

	std::unordered_map<std::string, size_t> m_edgesIndex;
	std::vector<StorageEdge> m_edges;

	std::unordered_map<std::string, StorageLocalSymbol> m_localSymbols;

	std::unordered_map<std::string, StorageSourceLocation> m_sourceLocations;

	std::unordered_set<std::string> m_serializedOccurrences; // this is used to prevent duplicates (unique)
	std::vector<StorageOccurrence> m_occurrences;

	std::unordered_set<std::string> m_serializedComponentAccesses; // this is used to prevent duplicates (unique)
	std::vector<StorageComponentAccess> m_componentAccesses;

	std::unordered_set<std::string> m_serializedCommentLocations; // this is used to prevent duplicates (unique)
	std::vector<StorageCommentLocation> m_commentLocations;

	std::unordered_set<std::string> m_serializedErrors; // this is used to prevent duplicates (unique)
	std::vector<StorageError> m_errors;

	Id m_nextId;
};

#endif // INTERMEDIATE_STORAGE_H
