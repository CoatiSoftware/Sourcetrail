#include "data/storage/Storage.h"

#include <unordered_map>

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
#include "utility/tracing.h"

Storage::Storage()
{
}

Storage::~Storage()
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

	std::unordered_map<Id, Id> injectedIdToOwnId;

	injected->forEachNode(
		[&](const StorageNode& injectedData)
		{
			const Id ownId = addNode(injectedData);
			if (ownId != 0)
			{
				injectedIdToOwnId[injectedData.id] = ownId;
			}
		}
	);

	injected->forEachFile(
		[&](const StorageFile& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.id);
			if (it != injectedIdToOwnId.end())
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
			it = injectedIdToOwnId.find(injectedData.id);
			if (it != injectedIdToOwnId.end())
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
			it = injectedIdToOwnId.find(injectedData.sourceNodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownSourceId = it->second;

			it = injectedIdToOwnId.find(injectedData.targetNodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownTargetId = it->second;

			const Id ownId = addEdge(StorageEdgeData(injectedData.type, ownSourceId, ownTargetId));

			if (ownId != 0)
			{
				injectedIdToOwnId[injectedData.id] = ownId;
			}
		}
	);

	injected->forEachLocalSymbol(
		[&](const StorageLocalSymbol& injectedData)
		{
			const Id ownId = addLocalSymbol(injectedData);
			if (ownId != 0)
			{
				injectedIdToOwnId[injectedData.id] = ownId;
			}
		}
	);

	injected->forEachSourceLocation(
		[&](const StorageSourceLocation& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.fileNodeId);
			if (it != injectedIdToOwnId.end())
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
					injectedIdToOwnId[injectedData.id] = ownId;
				}
			}
		}
	);

	injected->forEachOccurrence(
		[&](const StorageOccurrence& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.elementId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownElementId = it->second;

			it = injectedIdToOwnId.find(injectedData.sourceLocationId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownSourceLocationId = it->second;

			addOccurrence(StorageOccurrence(ownElementId, ownSourceLocationId));
		}
	);

	injected->forEachComponentAccess(
		[&](const StorageComponentAccessData& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.nodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownNodeId = it->second;

			addComponentAccess(StorageComponentAccessData(ownNodeId, injectedData.type));
		}
	);

	injected->forEachCommentLocation(
		[&](const StorageCommentLocationData& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.fileNodeId);
			if (it == injectedIdToOwnId.end())
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
