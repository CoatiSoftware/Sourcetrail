#include "data/IntermediateStorage.h"

#include "data/graph/Edge.h"
#include "utility/logging/logging.h"

IntermediateStorage::IntermediateStorage()
	: m_nextId(1)
{
}

IntermediateStorage::~IntermediateStorage()
{
}

Id IntermediateStorage::addEdge(int type, Id sourceId, Id targetId)
{
	std::shared_ptr<StorageEdge> edge = std::make_shared<StorageEdge>(0, type, sourceId, targetId);

	std::string serialized = serialize(*(edge.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_edgeNamesToIds.find(serialized);
	if (it != m_edgeNamesToIds.end())
	{
		return it->second;
	}

	Id id = m_nextId++;
	m_edgeNamesToIds[serialized] = id;
	m_edgeIdsToData[id] = edge;

	if (type == Edge::EDGE_MEMBER)
	{
		m_nodeIdsToMemberEdgeIds[targetId] = id;
	}

	return id;
}

Id IntermediateStorage::addNode(int type, const NameHierarchy& nameHierarchy, bool defined)
{
	std::shared_ptr<StorageNode> node = std::make_shared<StorageNode>(0, type, NameHierarchy::serialize(nameHierarchy), defined);

	std::string serialized = serialize(*(node.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_nodeNamesToIds.find(serialized);
	if (it != m_nodeNamesToIds.end())
	{
		// refine stored information
		if (defined)
		{
			std::unordered_map<Id, std::shared_ptr<StorageNode>>::const_iterator it2 = m_nodeIdsToData.find(it->second);
			std::shared_ptr<StorageNode> storageNode = it2->second;
			if (!storageNode->defined && storageNode->type < type)
			{
				storageNode->type = type;
			}
			storageNode->defined = true;
		}

		return it->second;
	}

	Id id = m_nextId++;
	m_nodeNamesToIds[serialized] = id;
	m_nodeIdsToData[id] = node;

	return id;
}

Id IntermediateStorage::addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime)
{
	std::shared_ptr<StorageFile> file = std::make_shared<StorageFile>(0, name, filePath, modificationTime);

	std::string serialized = serialize(*(file.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_fileNamesToIds.find(serialized);
	if (it != m_fileNamesToIds.end())
	{
		Id id = it->second;
		if (m_fileIdsToData[id]->filePath.size() == 0) // stored information is incomplete.
		{
			m_fileIdsToData[id] = file; // so we replace it.
		}
		return id;
	}

	Id id = m_nextId++;
	m_fileNamesToIds[serialized] = id;
	m_fileIdsToData[id] = file;

	return id;
}

Id IntermediateStorage::addFile(const std::string& filePath)
{
	std::shared_ptr<StorageFile> file = std::make_shared<StorageFile>(0, "", filePath, "");

	std::string serialized = serialize(*(file.get()));
	std::unordered_map<std::string, Id>::const_iterator it = m_fileNamesToIds.find(serialized);
	if (it != m_fileNamesToIds.end())
	{
		return it->second;
	}

	Id id = m_nextId++;
	m_fileNamesToIds[serialized] = id;
	m_fileIdsToData[id] = file;

	return id;
}

void IntermediateStorage::addSourceLocation(Id elementId, const ParseLocation& location, bool isScope)
{
	Id fileNodeId = addFile(location.filePath.str());
	m_sourceLocations.push_back(StorageSourceLocation(
		0,
		elementId,
		fileNodeId,
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber,
		isScope
	));
}

void IntermediateStorage::addComponentAccess(Id nodeId, int type)
{
	std::unordered_map<Id, Id>::const_iterator it = m_nodeIdsToMemberEdgeIds.find(nodeId);
	if (it != m_nodeIdsToMemberEdgeIds.end())
	{
		m_componentAccesses.push_back(StorageComponentAccess(it->second, type));
	}
	else
	{
		LOG_ERROR_STREAM(<< "Cannot assign access" << type << " to node id " << nodeId << " because it's not a child node.");
	}
}

void IntermediateStorage::addCommentLocation(const ParseLocation& location)
{
	Id fileNodeId = addFile(location.filePath.str());
	m_commentLocations.push_back(StorageCommentLocation(
		0,
		fileNodeId,
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber
	));
}

void IntermediateStorage::addError(const std::string& message, bool fatal, const ParseLocation& location)
{
	m_errors.push_back(StorageError(
		message,
		fatal,
		location.filePath.str(),
		location.startLineNumber,
		location.startColumnNumber
	));
}

void IntermediateStorage::transferToStorage(SqliteStorage& storage)
{
	storage.beginTransaction();

	std::unordered_map<Id, Id> clientIdToStorageId;

	for (std::unordered_map<Id, std::shared_ptr<StorageFile>>::const_iterator it = m_fileIdsToData.begin(); it != m_fileIdsToData.end(); it++)
	{
		if (it->second->name.size() > 0)
		{
			Id fileNodeId = storage.getFileByPath(it->second->filePath).id;
			if (fileNodeId == 0)
			{
				NameHierarchy nameHierarchy;
				nameHierarchy.push(std::make_shared<NameElement>(it->second->name));

				fileNodeId = storage.addFile(
					NameHierarchy::serialize(nameHierarchy),
					it->second->filePath,
					it->second->modificationTime
				);
			}
			clientIdToStorageId[it->first] = fileNodeId;
		}
	}

	for (std::unordered_map<Id, std::shared_ptr<StorageNode>>::const_iterator it = m_nodeIdsToData.begin(); it != m_nodeIdsToData.end(); it++)
	{
		StorageNode clientNode = *(it->second.get());
		StorageNode storageNode = storage.getNodeBySerializedName(clientNode.serializedName);
		Id storageNodeId = storageNode.id;
		if (storageNodeId)
		{
			if (clientNode.defined)
			{
				storage.setNodeDefined(true, storageNode.id);
				if (!storageNode.defined && storageNode.type < clientNode.type)
				{
					storage.setNodeType(clientNode.type, storageNode.id);
				}
			}
		}
		else
		{
			storageNodeId = storage.addNode(clientNode.type, clientNode.serializedName, clientNode.defined);
		}
		clientIdToStorageId[it->first] = storageNodeId;
	}

	for (std::unordered_map<Id, std::shared_ptr<StorageEdge>>::const_iterator it = m_edgeIdsToData.begin(); it != m_edgeIdsToData.end(); it++)
	{
		std::unordered_map<Id, Id>::const_iterator it2;
		it2 = clientIdToStorageId.find(it->second->sourceNodeId);
		if (it2 == clientIdToStorageId.end())
		{
			continue;
		}
		Id storageSourceId = it2->second;

		it2 = clientIdToStorageId.find(it->second->targetNodeId);
		if (it2 == clientIdToStorageId.end())
		{
			continue;
		}
		Id storageTargetId = it2->second;

		Id edgeId = storage.getEdgeBySourceTargetType(storageSourceId, storageTargetId, it->second->type).id;

		if (!edgeId)
		{
			edgeId = storage.addEdge(it->second->type, storageSourceId, storageTargetId);
		}
		clientIdToStorageId[it->first] = edgeId;
	}

	for (size_t i = 0; i < m_sourceLocations.size(); i++)
	{
		StorageSourceLocation sourceLocation = m_sourceLocations[i];
		std::unordered_map<Id, Id>::const_iterator it;
		it = clientIdToStorageId.find(sourceLocation.elementId);
		if (it == clientIdToStorageId.end())
		{
			continue;
		}
		Id storageElementId = it->second;

		it = clientIdToStorageId.find(sourceLocation.fileNodeId);
		if (it == clientIdToStorageId.end())
		{
			continue;
		}
		Id storageFileNodeId = it->second;

		storage.addSourceLocation(
			storageElementId,
			storageFileNodeId,
			sourceLocation.startLine,
			sourceLocation.startCol,
			sourceLocation.endLine,
			sourceLocation.endCol,
			sourceLocation.isScope
		);
	}

	for (size_t i = 0; i < m_componentAccesses.size(); i++)
	{
		StorageComponentAccess componentAccess = m_componentAccesses[i];

		std::unordered_map<Id, Id>::const_iterator it;
		it = clientIdToStorageId.find(componentAccess.memberEdgeId);
		if (it == clientIdToStorageId.end())
		{
			continue;
		}
		Id storageMemberEdgeId = it->second;

		storage.addComponentAccess(storageMemberEdgeId, componentAccess.type);
	}

	for (size_t i = 0; i < m_commentLocations.size(); i++)
	{
		StorageCommentLocation commentLocation = m_commentLocations[i];

		std::unordered_map<Id, Id>::const_iterator it;
		it = clientIdToStorageId.find(commentLocation.fileNodeId);
		if (it == clientIdToStorageId.end())
		{
			continue;
		}
		Id storageFileNodeId = it->second;

		storage.addCommentLocation(
			storageFileNodeId,
			commentLocation.startLine,
			commentLocation.startCol,
			commentLocation.endLine,
			commentLocation.endCol
		);
	}

	for (size_t i = 0; i < m_errors.size(); i++)
	{
		StorageError error = m_errors[i];

		storage.addError(
			error.message,
			error.fatal,
			error.filePath,
			error.lineNumber,
			error.columnNumber
		);
	}

	storage.commitTransaction();
}

void IntermediateStorage::forEachFile(std::function<void(const Id /*id*/, const StorageFile& /*data*/)> callback) const
{
	for (std::unordered_map<Id, std::shared_ptr<StorageFile>>::const_iterator it = m_fileIdsToData.begin(); it != m_fileIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachNode(std::function<void(const Id /*id*/, const StorageNode& /*data*/)> callback) const
{
	for (std::unordered_map<Id, std::shared_ptr<StorageNode>>::const_iterator it = m_nodeIdsToData.begin(); it != m_nodeIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachEdge(std::function<void(const Id /*id*/, const StorageEdge& /*data*/)> callback) const
{
	for (std::unordered_map<Id, std::shared_ptr<StorageEdge>>::const_iterator it = m_edgeIdsToData.begin(); it != m_edgeIdsToData.end(); it++)
	{
		callback(it->first, *(it->second.get()));
	}
}

void IntermediateStorage::forEachSourceLocation(std::function<void(const StorageSourceLocation& /*data*/)> callback) const
{
	for (std::vector<StorageSourceLocation>::const_iterator it = m_sourceLocations.begin(); it != m_sourceLocations.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachComponentAccess(std::function<void(const StorageComponentAccess& /*data*/)> callback) const
{
	for (std::vector<StorageComponentAccess>::const_iterator it = m_componentAccesses.begin(); it != m_componentAccesses.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachCommentLocation(std::function<void(const StorageCommentLocation& /*data*/)> callback) const
{
	for (std::vector<StorageCommentLocation>::const_iterator it = m_commentLocations.begin(); it != m_commentLocations.end(); it++)
	{
		callback(*it);
	}
}

void IntermediateStorage::forEachError(std::function<void(const StorageError& /*data*/)> callback) const
{
	for (std::vector<StorageError>::const_iterator it = m_errors.begin(); it != m_errors.end(); it++)
	{
		callback(*it);
	}
}

std::string IntermediateStorage::serialize(const StorageEdge& edge)
{
	return (
		std::to_string(edge.type) +
		std::to_string(edge.sourceNodeId) +
		std::to_string(edge.targetNodeId)
	);
}

std::string IntermediateStorage::serialize(const StorageNode& node)
{
	return node.serializedName;
}

std::string IntermediateStorage::serialize(const StorageFile& file)
{
	return file.filePath;
}
