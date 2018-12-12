#include "Storage.h"

#include "logging.h"
#include "tracing.h"

Storage::Storage()
{
}

void Storage::inject(Storage* injected)
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	std::map<Id, Id> injectedIdToOwnElementId;
	std::map<Id, Id> injectedIdToOwnSourceLocationId;

	TRACE();
	startInjection();

	{
		// TRACE("inject errors");

		for (const StorageError& error : injected->getErrors())
		{
			Id errorId = addError(error);
			injectedIdToOwnElementId.emplace(error.id, errorId);
		}
	}

	{
		// TRACE("inject nodes");

		const std::vector<StorageNode>& nodes = injected->getStorageNodes();

		std::vector<Id> nodeIds = addNodes(nodes);

		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (nodeIds[i])
			{
				injectedIdToOwnElementId.emplace(nodes[i].id, nodeIds[i]);
			}
		}
	}

	{
		// TRACE("inject files");

		for (const StorageFile& file : injected->getStorageFiles())
		{
			auto it = injectedIdToOwnElementId.find(file.id);
			if (it != injectedIdToOwnElementId.end())
			{
				addFile(StorageFile(
					it->second,
					file.filePath,
					file.languageIdentifier,
					file.modificationTime,
					file.indexed,
					file.complete
				));
			}
		}
	}

	{
		// TRACE("inject symbols");

		std::vector<StorageSymbol> symbols = injected->getStorageSymbols();
		for (size_t i = 0; i < symbols.size(); i++)
		{
			auto it = injectedIdToOwnElementId.find(symbols[i].id);
			if (it != injectedIdToOwnElementId.end())
			{
				symbols[i].id = it->second;
			}
			else
			{
				LOG_WARNING("New symbol id could not be found.");
				symbols.erase(symbols.begin() + i);
				i--;
			}
		}

		addSymbols(symbols);
	}

	{
		// TRACE("inject edges");

		std::vector<StorageEdge> edges = injected->getStorageEdges();
		for (size_t i = 0; i < edges.size(); i++)
		{
			StorageEdge& edge = edges[i];
			size_t updateCount = 0;

			auto it = injectedIdToOwnElementId.find(edge.sourceNodeId);
			if (it != injectedIdToOwnElementId.end())
			{
				edge.sourceNodeId = it->second;
				updateCount++;
			}

			it = injectedIdToOwnElementId.find(edge.targetNodeId);
			if (it != injectedIdToOwnElementId.end())
			{
				edge.targetNodeId = it->second;
				updateCount++;
			}

			if (updateCount != 2)
			{
				LOG_WARNING("New edge source or target id could not be found.");
				edges.erase(edges.begin() + i);
				i--;
			}
		}

		std::vector<Id> edgeIds = addEdges(edges);

		if (edges.size() == edgeIds.size())
		{
			for (size_t i = 0; i < edgeIds.size(); i++)
			{
				if (edgeIds[i])
				{
					injectedIdToOwnElementId.emplace(edges[i].id, edgeIds[i]);
				}
			}
		}
		else
		{
			LOG_ERROR("Returned edge ids don't match injected count.");
		}
	}

	{
		// TRACE("inject local symbols");

		const std::set<StorageLocalSymbol>& symbols = injected->getStorageLocalSymbols();
		std::vector<Id> symbolIds = addLocalSymbols(symbols);

		auto it = symbols.begin();
		for (size_t i = 0; i < symbols.size(); i++)
		{
			if (symbolIds[i])
			{
				injectedIdToOwnElementId.emplace(it->id, symbolIds[i]);
			}
			it++;
		}
	}

	{
		// TRACE("inject locations");

		const std::set<StorageSourceLocation>& oldLocations = injected->getStorageSourceLocations();
		std::vector<StorageSourceLocation> locations;
		locations.reserve(oldLocations.size());

		for (const StorageSourceLocation& location : oldLocations)
		{
			auto it = injectedIdToOwnElementId.find(location.fileNodeId);
			if (it != injectedIdToOwnElementId.end())
			{
				const Id ownFileNodeId = it->second;
				locations.emplace_back(
					location.id,
					ownFileNodeId,
					location.startLine,
					location.startCol,
					location.endLine,
					location.endCol,
					location.type
				);
			}
		}

		std::vector<Id> locationIds = addSourceLocations(locations);

		if (locations.size() == locationIds.size())
		{
			for (size_t i = 0; i < locationIds.size(); i++)
			{
				if (locationIds[i])
				{
					injectedIdToOwnSourceLocationId.emplace(locations[i].id, locationIds[i]);
				}
			}
		}
		else
		{
			LOG_ERROR("Returned source locations ids don't match injected count.");
		}
	}

	{
		// TRACE("inject occurrences");

		const std::set<StorageOccurrence>& oldOccurences = injected->getStorageOccurrences();

		std::vector<StorageOccurrence> occurrences;
		occurrences.reserve(oldOccurences.size());

		for (const StorageOccurrence& occurrence : oldOccurences)
		{
			Id elementId = 0;
			Id sourceLocationId = 0;

			auto it = injectedIdToOwnElementId.find(occurrence.elementId);
			if (it != injectedIdToOwnElementId.end())
			{
				elementId = it->second;
			}

			it = injectedIdToOwnSourceLocationId.find(occurrence.sourceLocationId);
			if (it != injectedIdToOwnSourceLocationId.end())
			{
				sourceLocationId = it->second;
			}

			if (elementId && sourceLocationId)
			{
				occurrences.emplace_back(elementId, sourceLocationId);
			}
			else
			{
				LOG_WARNING("New occurrence element or location id could not be found.");
			}
		}

		addOccurrences(occurrences);
	}

	{
		// TRACE("inject accesses");

		const std::set<StorageComponentAccess>& oldAccesses = injected->getComponentAccesses();
		std::vector<StorageComponentAccess> accesses;
		accesses.reserve(oldAccesses.size());

		for (const StorageComponentAccess& access : oldAccesses)
		{
			auto it = injectedIdToOwnElementId.find(access.nodeId);
			if (it != injectedIdToOwnElementId.end())
			{
				accesses.emplace_back(it->second, access.type);
			}
		}

		addComponentAccesses(accesses);
	}

	finishInjection();
}

void Storage::startInjection()
{
	// may be implemented in derived
}

void Storage::finishInjection()
{
	// may be implemented in derived
}
