#include "data/Storage.h"

#include <unordered_map>

#include "data/StorageTypes.h"
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

	std::unordered_map<Id, Id> injectedIdToOwnId;

	injected->forEachNode(
		[&](const StorageNode& injectedData)
		{
			const Id ownId = addNode(injectedData.type, injectedData.serializedName);
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
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownId = it->second;

			addFile(ownId, injectedData.filePath, injectedData.modificationTime, injectedData.complete);
		}
	);

	injected->forEachSymbol(
		[&](const StorageSymbol& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.id);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownId = it->second;

			addSymbol(ownId, injectedData.definitionKind);
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

			const Id ownId = addEdge(injectedData.type, ownSourceId, ownTargetId);

			if (ownId != 0)
			{
				injectedIdToOwnId[injectedData.id] = ownId;
			}
		}
	);

	injected->forEachLocalSymbol(
		[&](const StorageLocalSymbol& injectedData)
		{
			const Id ownId = addLocalSymbol(injectedData.name);
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
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownFileNodeId = it->second;

			const Id ownId = addSourceLocation(
				ownFileNodeId,
				injectedData.startLine,
				injectedData.startCol,
				injectedData.endLine,
				injectedData.endCol,
				injectedData.type
			);
			if (ownId != 0)
			{
				injectedIdToOwnId[injectedData.id] = ownId;
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

			addOccurrence(ownElementId, ownSourceLocationId);
		}
	);

	injected->forEachComponentAccess(
		[&](const StorageComponentAccess& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.nodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownNodeId = it->second;

			addComponentAccess(ownNodeId, injectedData.type);
		}
	);

	injected->forEachCommentLocation(
		[&](const StorageCommentLocation& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.fileNodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			const Id ownFileNodeId = it->second;

			addCommentLocation(
				ownFileNodeId,
				injectedData.startLine,
				injectedData.startCol,
				injectedData.endLine,
				injectedData.endCol
			);
		}
	);

	injected->forEachError(
		[&](const StorageError& injectedData)
		{
			addError(
				injectedData.message,
				injectedData.filePath,
				injectedData.lineNumber,
				injectedData.columnNumber,
				injectedData.fatal,
				injectedData.indexed
			);
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
