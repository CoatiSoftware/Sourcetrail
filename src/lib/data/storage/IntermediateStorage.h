#ifndef INTERMEDIATE_STORAGE_H
#define INTERMEDIATE_STORAGE_H

#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>

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
#include "data/storage/Storage.h"

class IntermediateStorage: public Storage
{
public:
	IntermediateStorage();

	void clear();

	size_t getByteSize(size_t stringSize) const;
	size_t getSourceLocationCount() const;

	void setAllFilesIncomplete();
	void setFilesWithErrorsIncomplete();

	Id addNode(const StorageNodeData& nodeData) override;
	void addSymbol(const StorageSymbol& symbol) override;
	void addFile(const StorageFile& file) override;
	Id addEdge(const StorageEdgeData& edgeData) override;
	Id addLocalSymbol(const StorageLocalSymbolData& localSymbolData) override;
	Id addSourceLocation(const StorageSourceLocationData& sourceLocationData) override;
	void addOccurrence(const StorageOccurrence& occurrence) override;
	void addOccurrences(const std::vector<StorageOccurrence>& occurrences) override;
	void addComponentAccess(const StorageComponentAccess& componentAccess) override;
	void addCommentLocation(const StorageCommentLocationData& commentLocationData) override;
	void addError(const StorageErrorData& errorData) override;

	void forEachNode(std::function<void(const StorageNode& /*data*/)> callback) const override;
	void forEachFile(std::function<void(const StorageFile& /*data*/)> callback) const override;
	void forEachSymbol(std::function<void(const StorageSymbol& /*data*/)> callback) const override;
	void forEachEdge(std::function<void(const StorageEdge& /*data*/)> callback) const override;
	void forEachLocalSymbol(std::function<void(const StorageLocalSymbol& /*data*/)> callback) const override;
	void forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const override;
	void forEachOccurrence(std::function<void(const StorageOccurrence& /*data*/)> callback) const override;
	void forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const override;
	void forEachCommentLocation(std::function<void(const StorageCommentLocationData& /*data*/)> callback) const override;
	void forEachError(std::function<void(const StorageErrorData& /*data*/)> callback) const override;

	// for conversion to and from 'SharedIntermediateStorage'

	const std::vector<StorageNode>& getStorageNodes() const;
	const std::vector<StorageFile>& getStorageFiles() const;
	const std::vector<StorageSymbol>& getStorageSymbols() const;
	const std::vector<StorageEdge>& getStorageEdges() const;
	std::vector<StorageLocalSymbol> getStorageLocalSymbols() const;
	std::vector<StorageSourceLocation> getStorageSourceLocations() const;
	const std::vector<StorageOccurrence>& getStorageOccurrences() const;
	const std::vector<StorageComponentAccess>& getComponentAccesses() const;
	const std::vector<StorageCommentLocationData>& getCommentLocations() const;
	const std::vector<StorageErrorData>& getErrors() const;

	void setStorageNodes(const std::vector<StorageNode>& storageNodes);
	void setStorageFiles(const std::vector<StorageFile>& storageFiles);
	void setStorageSymbols(const std::vector<StorageSymbol>& storageSymbols);
	void setStorageEdges(const std::vector<StorageEdge>& storageEdges);
	void setStorageLocalSymbols(const std::vector<StorageLocalSymbol>& storageLocalSymbols);
	void setStorageSourceLocations(const std::vector<StorageSourceLocation>& storageSourceLocations);
	void setStorageOccurrences(const std::vector<StorageOccurrence>& storageOccurrences);
	void setComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses);
	void setCommentLocations(const std::vector<StorageCommentLocationData>& commentLocations);
	void setErrors(const std::vector<StorageErrorData>& errors);

	Id getNextId() const;
	void setNextId(const Id nextId);

private:
	std::wstring serialize(const StorageNodeData& nodeData) const;
	std::wstring serialize(const StorageFile& file) const;
	std::wstring serialize(const StorageEdgeData& edgeData) const;
	std::wstring serialize(const StorageLocalSymbolData& localSymbolData) const;
	std::wstring serialize(const StorageSourceLocationData& sourceLocationData) const;
	std::wstring serialize(const StorageOccurrence& occurrence) const;
	std::wstring serialize(const StorageComponentAccess& componentAccessData) const;
	std::wstring serialize(const StorageCommentLocationData& commentLocationData) const;
	std::wstring serialize(const StorageErrorData& errorData) const;

	std::unordered_map<std::wstring, size_t> m_nodesIndex;
	std::vector<StorageNode> m_nodes;

	std::unordered_map<std::wstring, size_t> m_serializedFiles; // this is used to prevent duplicates (unique)
	std::vector<StorageFile> m_files;

	std::vector<StorageSymbol> m_symbols;

	std::unordered_map<std::wstring, size_t> m_edgesIndex;
	std::vector<StorageEdge> m_edges;

	std::unordered_map<std::wstring, StorageLocalSymbol> m_localSymbols;

	std::unordered_map<std::wstring, StorageSourceLocation> m_sourceLocations;

	std::unordered_set<std::wstring> m_serializedOccurrences; // this is used to prevent duplicates (unique)
	std::vector<StorageOccurrence> m_occurrences;

	std::unordered_set<std::wstring> m_serializedComponentAccesses; // this is used to prevent duplicates (unique)
	std::vector<StorageComponentAccess> m_componentAccesses;

	std::unordered_set<std::wstring> m_serializedCommentLocations; // this is used to prevent duplicates (unique)
	std::vector<StorageCommentLocationData> m_commentLocations;

	std::unordered_set<std::wstring> m_serializedErrors; // this is used to prevent duplicates (unique)
	std::vector<StorageErrorData> m_errors;

	Id m_nextId;
};

#endif // INTERMEDIATE_STORAGE_H
