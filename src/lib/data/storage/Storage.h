#ifndef STORAGE_H
#define STORAGE_H

#include <functional>
#include <mutex>
#include <set>
#include <string>

#include "StorageComponentAccess.h"
#include "StorageEdge.h"
#include "StorageError.h"
#include "StorageElementComponent.h"
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
	virtual std::vector<Id> addNodes(const std::vector<StorageNode>& nodes) = 0;
	virtual void addSymbol(const StorageSymbol& data) = 0;
	virtual void addSymbols(const std::vector<StorageSymbol>& symbols) = 0;
	virtual void addFile(const StorageFile& data) = 0;
	virtual Id addEdge(const StorageEdgeData& data) = 0;
	virtual std::vector<Id> addEdges(const std::vector<StorageEdge>& edges) = 0;
	virtual Id addLocalSymbol(const StorageLocalSymbolData& data) = 0;
	virtual std::vector<Id> addLocalSymbols(const std::set<StorageLocalSymbol>& symbols) = 0;
	virtual Id addSourceLocation(const StorageSourceLocationData& data) = 0;
	virtual std::vector<Id> addSourceLocations(const std::vector<StorageSourceLocation>& locations) = 0;
	virtual void addOccurrence(const StorageOccurrence& data) = 0;
	virtual void addOccurrences(const std::vector<StorageOccurrence>& occurrences) = 0;
	virtual void addComponentAccess(const StorageComponentAccess& componentAccess) = 0;
	virtual void addComponentAccesses(const std::vector<StorageComponentAccess>& componentAccesses) = 0;
	virtual void addElementComponent(const StorageElementComponent& component) = 0;
	virtual void addElementComponents(const std::vector<StorageElementComponent>& components) = 0;
	virtual Id addError(const StorageErrorData& data) = 0;

	virtual const std::vector<StorageNode>& getStorageNodes() const = 0;
	virtual const std::vector<StorageFile>& getStorageFiles() const = 0;
	virtual const std::vector<StorageSymbol>& getStorageSymbols() const = 0;
	virtual const std::vector<StorageEdge>& getStorageEdges() const = 0;
	virtual const std::set<StorageLocalSymbol>& getStorageLocalSymbols() const = 0;
	virtual const std::set<StorageSourceLocation>& getStorageSourceLocations() const = 0;
	virtual const std::set<StorageOccurrence>& getStorageOccurrences() const = 0;
	virtual const std::set<StorageComponentAccess>& getComponentAccesses() const = 0;
	virtual const std::set<StorageElementComponent>& getElementComponents() const = 0;
	virtual const std::vector<StorageError>& getErrors() const = 0;

	void inject(Storage* injected);

private:
	virtual void startInjection();
	virtual void finishInjection();

	std::mutex m_dataMutex;
};

#endif // STORAGE_H
