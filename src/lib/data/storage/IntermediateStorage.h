#ifndef INTERMEDIATE_STORAGE_H
#define INTERMEDIATE_STORAGE_H

#include <memory>
#include <map>
#include <set>

#include "Storage.h"

class IntermediateStorage
	: public Storage
{
public:
	IntermediateStorage();

	void clear();

	size_t getByteSize(size_t stringSize) const;
	size_t getSourceLocationCount() const;

	bool hasFatalErrors() const;
	void setAllFilesIncomplete();
	void setFilesWithErrorsIncomplete();

	std::pair<Id, bool> addNode(const StorageNodeData& nodeData) override;
	std::vector<Id> addNodes(const std::vector<StorageNode>& nodes) override;
	void setNodeType(Id nodeId, int nodeType);
	void addSymbol(const StorageSymbol& symbol) override;
	void addSymbols(const std::vector<StorageSymbol>& symbols) override;
	void addFile(const StorageFile& file) override;
	void setFileLanguage(Id fileId, const std::wstring& languageIdentifier);
	Id addEdge(const StorageEdgeData& edgeData) override;
	std::vector<Id> addEdges(const std::vector<StorageEdge>& edges) override;
	Id addLocalSymbol(const StorageLocalSymbolData& localSymbolData) override;
	std::vector<Id> addLocalSymbols(const std::set<StorageLocalSymbol>& symbols) override;
	Id addSourceLocation(const StorageSourceLocationData& sourceLocationData) override;
	std::vector<Id> addSourceLocations(const std::vector<StorageSourceLocation>& locations) override;
	void addOccurrence(const StorageOccurrence& occurrence) override;
	void addOccurrences(const std::vector<StorageOccurrence>& occurrences) override;
	void addComponentAccess(const StorageComponentAccess& componentAccess) override;
	void addComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses) override;
	Id addError(const StorageErrorData& errorData) override;

	const std::vector<StorageNode>& getStorageNodes() const override;
	const std::vector<StorageFile>& getStorageFiles() const override;
	const std::vector<StorageSymbol>& getStorageSymbols() const override;
	const std::vector<StorageEdge>& getStorageEdges() const override;
	const std::set<StorageLocalSymbol>& getStorageLocalSymbols() const override;
	const std::set<StorageSourceLocation>& getStorageSourceLocations() const override;
	const std::set<StorageOccurrence>& getStorageOccurrences() const override;
	const std::set<StorageComponentAccess>& getComponentAccesses() const override;
	const std::vector<StorageError>& getErrors() const override;

	void setStorageNodes(std::vector<StorageNode> storageNodes);
	void setStorageFiles(std::vector<StorageFile> storageFiles);
	void setStorageSymbols(std::vector<StorageSymbol> storageSymbols);
	void setStorageEdges(std::vector<StorageEdge> storageEdges);
	void setStorageLocalSymbols(std::set<StorageLocalSymbol> storageLocalSymbols);
	void setStorageSourceLocations(std::set<StorageSourceLocation> storageSourceLocations);
	void setStorageOccurrences(std::set<StorageOccurrence> storageOccurrences);
	void setComponentAccesses(std::set<StorageComponentAccess> componentAccesses);
	void setErrors(std::vector<StorageError> errors);

	Id getNextId() const;
	void setNextId(const Id nextId);

private:
	std::map<StorageNodeData, size_t> m_nodesIndex;
	std::map<Id, size_t> m_nodeIdIndex;
	std::vector<StorageNode> m_nodes;

	std::map<StorageFile, size_t> m_filesIndex; // this is used to prevent duplicates (unique)
	std::map<Id, size_t> m_filesIdIndex;
	std::vector<StorageFile> m_files;
	
	std::vector<StorageSymbol> m_symbols;

	std::map<StorageEdgeData, size_t> m_edgesIndex;
	std::vector<StorageEdge> m_edges;

	std::set<StorageLocalSymbol> m_localSymbols;

	std::set<StorageSourceLocation> m_sourceLocations;

	std::set<StorageOccurrence> m_occurrences;

	std::set<StorageComponentAccess> m_componentAccesses;

	std::map<StorageErrorData, size_t> m_errorsIndex; // this is used to prevent duplicates (unique)
	std::vector<StorageError> m_errors;

	Id m_nextId;
};

#endif // INTERMEDIATE_STORAGE_H
