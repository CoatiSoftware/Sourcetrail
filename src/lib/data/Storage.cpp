#include "data/Storage.h"

#include <unordered_map>

#include "data/graph/Edge.h"
#include "data/StorageTypes.h"
#include "utility/logging/logging.h"
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

	injected->forEachFile(
		[&](Id injectedId, const StorageFile& injectedData)
		{
			if (injectedData.name.size() == 0)
			{
				return;
			}

			Id ownId = addFile(injectedData.name, injectedData.filePath, injectedData.modificationTime);
			if (ownId != 0)
			{
				injectedIdToOwnId[injectedId] = ownId;
			}
		}
	);

	injected->forEachNode(
		[&](Id injectedId, const StorageNode& injectedData)
		{
			Id ownId = addNode(injectedData.type, injectedData.serializedName, injectedData.definitionType);
			if (ownId != 0)
			{
				injectedIdToOwnId[injectedId] = ownId;
			}
		}
	);

	injected->forEachEdge(
		[&](Id injectedId, const StorageEdge& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.sourceNodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			Id ownSourceId = it->second;

			it = injectedIdToOwnId.find(injectedData.targetNodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			Id ownTargetId = it->second;

			Id ownId = addEdge(injectedData.type, ownSourceId, ownTargetId);

			if (ownId != 0)
			{
				injectedIdToOwnId[injectedId] = ownId;
			}
		}
	);

	injected->forEachLocalSymbol(
		[&](const Id injectedId, const StorageLocalSymbol& injectedData)
		{
			Id ownId = addLocalSymbol(injectedData.name);
			if (ownId != 0)
			{
				injectedIdToOwnId[injectedId] = ownId;
			}
		}
	);

	injected->forEachSourceLocation(
		[&](const StorageSourceLocation& injectedData)
		{
			std::unordered_map<Id, Id>::const_iterator it;
			it = injectedIdToOwnId.find(injectedData.elementId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			Id ownElementId = it->second;

			it = injectedIdToOwnId.find(injectedData.fileNodeId);
			if (it == injectedIdToOwnId.end())
			{
				return;
			}
			Id ownFileNodeId = it->second;

			addSourceLocation(
				ownElementId,
				ownFileNodeId,
				injectedData.startLine,
				injectedData.startCol,
				injectedData.endLine,
				injectedData.endCol,
				injectedData.type
			);
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
			Id ownNodeId = it->second;

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
			Id ownFileNodeId = it->second;

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
				injectedData.fatal,
				injectedData.indexed,
				injectedData.filePath,
				injectedData.lineNumber,
				injectedData.columnNumber
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
