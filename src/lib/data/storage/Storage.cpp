#include "Storage.h"

#include <unordered_map>

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
#include "tracing.h"

Storage::Storage()
{
}

void Storage::inject(Storage* injected)
{
	std::lock_guard<std::mutex> lock(m_dataMutex);

	TRACE();
	startInjection();

	injected->forEachError(
		[&](const StorageErrorData& injectedData)
		{
			addError(injectedData);
		}
	);

	std::unordered_map<Id, Id> injectedIdToOwnElementId;

	injected->forEachNode(
		[&](const StorageNode& injectedData)
		{
			const Id ownId = addNode(injectedData).first;
			if (ownId != 0)
			{
				injectedIdToOwnElementId[injectedData.id] = ownId;
			}
		}
	);

	injected->forEachFile(
		[&](const StorageFile& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnElementId.find(injectedData.id);
			if (it != injectedIdToOwnElementId.end())
			{
				const Id ownId = it->second;
				addFile(StorageFile(ownId, injectedData.filePath, injectedData.modificationTime, injectedData.indexed, injectedData.complete));
			}
		}
	);

	injected->forEachSymbol(
		[&](const StorageSymbol& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnElementId.find(injectedData.id);
			if (it != injectedIdToOwnElementId.end())
			{
				const Id ownId = it->second;
				addSymbol(StorageSymbol(ownId, injectedData.definitionKind));
			}
		}
	);

	injected->forEachEdge(
		[&](const StorageEdge& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnElementId.find(injectedData.sourceNodeId);
			if (it == injectedIdToOwnElementId.end())
			{
				return;
			}
			const Id ownSourceId = it->second;

			it = injectedIdToOwnElementId.find(injectedData.targetNodeId);
			if (it == injectedIdToOwnElementId.end())
			{
				return;
			}
			const Id ownTargetId = it->second;

			const Id ownId = addEdge(StorageEdgeData(injectedData.type, ownSourceId, ownTargetId));

			if (ownId != 0)
			{
				injectedIdToOwnElementId[injectedData.id] = ownId;
			}
		}
	);

	injected->forEachLocalSymbol(
		[&](const StorageLocalSymbol& injectedData)
		{
			const Id ownId = addLocalSymbol(injectedData);
			if (ownId != 0)
			{
				injectedIdToOwnElementId[injectedData.id] = ownId;
			}
		}
	);

	std::unordered_map<Id, Id> injectedIdToOwnSourceLocationId;

	injected->forEachSourceLocation(
		[&](const StorageSourceLocation& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnElementId.find(injectedData.fileNodeId);
			if (it != injectedIdToOwnElementId.end())
			{
				const Id ownFileNodeId = it->second;

				const Id ownId = addSourceLocation(StorageSourceLocationData(
					ownFileNodeId,
					injectedData.startLine,
					injectedData.startCol,
					injectedData.endLine,
					injectedData.endCol,
					injectedData.type
				));
				if (ownId != 0)
				{
					injectedIdToOwnSourceLocationId[injectedData.id] = ownId;
				}
			}
		}
	);

	{
		std::vector<StorageOccurrence> occurrences;
		injected->forEachOccurrence(
			[&](const StorageOccurrence& injectedData)
			{
				std::unordered_map<Id, Id>::const_iterator it;
				it = injectedIdToOwnElementId.find(injectedData.elementId);
				if (it == injectedIdToOwnElementId.end())
				{
					return;
				}
				const Id ownElementId = it->second;

				it = injectedIdToOwnSourceLocationId.find(injectedData.sourceLocationId);
				if (it == injectedIdToOwnSourceLocationId.end())
				{
					return;
				}
				const Id ownSourceLocationId = it->second;

				occurrences.emplace_back(ownElementId, ownSourceLocationId);
			}
		);
		addOccurrences(occurrences);
	}

	injected->forEachComponentAccess(
		[&](const StorageComponentAccess& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnElementId.find(injectedData.nodeId);
			if (it == injectedIdToOwnElementId.end())
			{
				return;
			}
			const Id ownNodeId = it->second;

			addComponentAccess(StorageComponentAccess(ownNodeId, injectedData.type));
		}
	);

	injected->forEachCommentLocation(
		[&](const StorageCommentLocationData& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnElementId.find(injectedData.fileNodeId);
			if (it == injectedIdToOwnElementId.end())
			{
				return;
			}
			const Id ownFileNodeId = it->second;

			addCommentLocation(StorageCommentLocationData(
				ownFileNodeId,
				injectedData.startLine,
				injectedData.startCol,
				injectedData.endLine,
				injectedData.endCol
			));
		}
	);

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
