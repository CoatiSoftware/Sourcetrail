#ifndef INTERMEDIATE_STORAGE_H
#define INTERMEDIATE_STORAGE_H

#include <memory>
#include <map>
#include <set>

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
#include "Storage.h"

class IntermediateStorage: public Storage
{
public:
	IntermediateStorage();

	void clear();

	size_t getByteSize(size_t stringSize) const;
	size_t getSourceLocationCount() const;

	void setAllFilesIncomplete();
	void setFilesWithErrorsIncomplete();

	std::pair<Id, bool> addNode(const StorageNodeData& nodeData) override;
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
	const std::set<StorageLocalSymbol>& getStorageLocalSymbols() const;
	const std::set<StorageSourceLocation>& getStorageSourceLocations() const;
	const std::set<StorageOccurrence>& getStorageOccurrences() const;
	const std::set<StorageComponentAccess>& getComponentAccesses() const;
	const std::set<StorageCommentLocationData>& getCommentLocations() const;
	const std::vector<StorageErrorData>& getErrors() const;

	void setStorageNodes(std::vector<StorageNode> storageNodes);
	void setStorageFiles(std::vector<StorageFile> storageFiles);
	void setStorageSymbols(std::vector<StorageSymbol> storageSymbols);
	void setStorageEdges(std::vector<StorageEdge> storageEdges);
	void setStorageLocalSymbols(std::set<StorageLocalSymbol> storageLocalSymbols);
	void setStorageSourceLocations(std::set<StorageSourceLocation> storageSourceLocations);
	void setStorageOccurrences(std::set<StorageOccurrence> storageOccurrences);
	void setComponentAccesses(std::set<StorageComponentAccess> componentAccesses);
	void setCommentLocations(std::set<StorageCommentLocationData> commentLocations);
	void setErrors(std::vector<StorageErrorData> errors);

	Id getNextId() const;
	void setNextId(const Id nextId);

private:
	std::wstring serialize(const StorageErrorData& errorData) const;

	std::map<StorageNodeData, size_t> m_nodesIndex;
	std::vector<StorageNode> m_nodes;

	std::map<StorageFile, size_t> m_filesIndex; // this is used to prevent duplicates (unique)
	std::vector<StorageFile> m_files;

	std::vector<StorageSymbol> m_symbols;

	std::map<StorageEdgeData, size_t> m_edgesIndex;
	std::vector<StorageEdge> m_edges;

	std::set<StorageLocalSymbol> m_localSymbols;

	std::set<StorageSourceLocation> m_sourceLocations;

	std::set<StorageOccurrence> m_occurrences;

	std::set<StorageComponentAccess> m_componentAccesses;

	std::set<StorageCommentLocationData> m_commentLocations;

	std::set<StorageErrorData> m_errorsIndex; // this is used to prevent duplicates (unique)
	std::vector<StorageErrorData> m_errors;

	Id m_nextId;
};

#endif // INTERMEDIATE_STORAGE_H
