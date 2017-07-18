#include "data/storage/StorageTransformationAnonymousTypedef.h"

#include <map>

#include "data/graph/Node.h"
#include "data/name/NameDelimiterType.h"
#include "data/storage/IntermediateStorage.h"
#include "utility/logging/logging.h"
#include "utility/utilityString.h"

void StorageTransformationAnonymousTypedef::transform(std::shared_ptr<IntermediateStorage> storage)
{
	LOG_INFO("Applying storage transformation to rename anonymous types inside typedefs.");

	if (!storage)
	{
		return;
	}

	std::vector<StorageNode> nodes = storage->getStorageNodes();
	std::vector<StorageEdge> edges = storage->getStorageEdges();
	std::vector<StorageOccurrence> occurrences = storage->getStorageOccurrences();

	std::map<Id, Id> nodesToMerge;
	std::map<std::string, std::string> nodesToRename;

	{
		std::map<Id, StorageNode> typedefNodes;
		std::map<Id, StorageNode> anonymousTypeNodes;

		for (const StorageNode& node : nodes)
		{
			const Node::NodeType nodeType = Node::intToType(node.type);
			if (nodeType & Node::NODE_TYPEDEF)
			{
				typedefNodes.insert(std::pair<Id, StorageNode>(node.id, node));
			}
			else if(nodeType & (Node::NODE_STRUCT | Node::NODE_CLASS | Node::NODE_ENUM | Node::NODE_UNION))
			{
				const NameHierarchy nameHierarchy = NameHierarchy::deserialize(node.serializedName);
				if (nameHierarchy.back() && utility::isPrefix("anonymous ", nameHierarchy.back()->getName()))
				{
					anonymousTypeNodes.insert(std::pair<Id, StorageNode>(node.id, node));
				}
			}
		}

		for (const StorageEdge& edge : edges)
		{
			if (Edge::intToType(edge.type) & Edge::EDGE_TYPE_USAGE)
			{
				std::map<Id, StorageNode>::const_iterator itAnonymousTypeNodes = anonymousTypeNodes.find(edge.targetNodeId);
				if (itAnonymousTypeNodes != anonymousTypeNodes.end())
				{
					std::map<Id, StorageNode>::const_iterator itTypedefNodes = typedefNodes.find(edge.sourceNodeId);
					if (itTypedefNodes != typedefNodes.end())
					{
						nodesToMerge.insert(std::pair<Id, Id>(itTypedefNodes->first, itAnonymousTypeNodes->first));
						nodesToRename.insert(std::pair<std::string, std::string>(itAnonymousTypeNodes->second.serializedName, itTypedefNodes->second.serializedName));
					}
				}
			}
		}
	}

	// remove typedef nodes that refer to anonymous types
	// and rename anonymous types to typedef name
	for (size_t i = 0; i < nodes.size(); i++)
	{
		if (nodesToMerge.find(nodes[i].id) != nodesToMerge.end())
		{
			nodes.erase(nodes.begin() + i);
			i--;
		}
		else
		{
			for (std::map<std::string, std::string>::const_iterator it = nodesToRename.begin(); it != nodesToRename.end(); it++)
			{
				if (utility::isPrefix(it->first, nodes[i].serializedName))
				{
					nodes[i].serializedName = it->second + nodes[i].serializedName.substr(it->first.size());
				}
			}
		}
	}

	// redirect edges that pointed to removed nodes
	for (StorageEdge& edge : edges)
	{
		std::map<Id, Id>::const_iterator it = nodesToMerge.find(edge.targetNodeId);
		if (it != nodesToMerge.end())
		{
			edge.targetNodeId = it->second;
		}
	}

	// relink source locations of removed nodes
	for (StorageOccurrence& occurrence : occurrences)
	{
		std::map<Id, Id>::const_iterator it = nodesToMerge.find(occurrence.elementId);
		if (it != nodesToMerge.end())
		{
			occurrence.elementId = it->second;
		}
	}

	storage->setStorageNodes(nodes);
	storage->setStorageEdges(edges);
	storage->setStorageOccurrences(occurrences);

	LOG_INFO("Renamed " + std::to_string(nodesToRename.size()) + " types.");
}
