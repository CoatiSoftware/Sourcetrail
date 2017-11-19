#include "cxxtest/TestSuite.h"

#include "data/storage/StorageTransformationAnonymousTypedef.h"
#include "data/storage/IntermediateStorage.h"
#include "data/graph/Node.h"
#include "utility/utility.h"

class StorageTransformationAnonymousTypedefTestSuite: public CxxTest::TestSuite
{
public:
	void test_transformation_removes_anonymous_class_that_has_typedef()
	{
		std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();

		Id anonymousTypeId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_CLASS), NameHierarchy::serialize(NameHierarchy("anonymous class (input.cc<1:9>)", NAME_DELIMITER_CXX))));
		Id typedefId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_TYPEDEF), NameHierarchy::serialize(NameHierarchy("ClassTypedef", NAME_DELIMITER_CXX))));
		storage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_TYPE_USAGE), typedefId, anonymousTypeId));

		StorageTransformationAnonymousTypedef::transform(storage);

		const std::vector<StorageNode> nodes = storage->getStorageNodes();
		std::vector<std::string> nodeNames(nodes.size());
		std::transform(nodes.begin(), nodes.end(), nodeNames.begin(), [](const StorageNode& node)
		{
			return NameHierarchy::deserialize(node.serializedName).getQualifiedNameWithSignature();
		});

		TS_ASSERT_EQUALS(1, nodeNames.size());
		TS_ASSERT(utility::containsElement<std::string>(
			nodeNames, "ClassTypedef"
		));
	}

	void test_transformation_redirects_incoming_edges_to_renamed_anonymous_class()
	{
		std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();

		Id anonymousTypeId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_CLASS), NameHierarchy::serialize(NameHierarchy("anonymous class (input.cc<1:9>)", NAME_DELIMITER_CXX))));
		Id typedefId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_TYPEDEF), NameHierarchy::serialize(NameHierarchy("ClassTypedef", NAME_DELIMITER_CXX))));
		storage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_TYPE_USAGE), typedefId, anonymousTypeId));
		storage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_TYPE_USAGE), 42, typedefId));

		StorageTransformationAnonymousTypedef::transform(storage);

		Id targetNodeId = 0;
		for (const StorageEdge& edge : storage->getStorageEdges())
		{
			if (edge.sourceNodeId == 42)
			{
				targetNodeId = edge.targetNodeId;
				break;
			}
		}

		std::string targetNodeName = "";
		for (const StorageNode& node : storage->getStorageNodes())
		{
			if (node.id == targetNodeId)
			{
				targetNodeName = NameHierarchy::deserialize(node.serializedName).getQualifiedNameWithSignature();
				break;
			}
		}

		TS_ASSERT_EQUALS(targetNodeName, "ClassTypedef");
	}

	void test_transformation_keeps_outgoing_edges_of_renamed_anonymous_class()
	{
		std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();

		Id anonymousTypeId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_CLASS), NameHierarchy::serialize(NameHierarchy("anonymous class (input.cc<1:9>)", NAME_DELIMITER_CXX))));
		Id typedefId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_TYPEDEF), NameHierarchy::serialize(NameHierarchy("ClassTypedef", NAME_DELIMITER_CXX))));
		storage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_TYPE_USAGE), typedefId, anonymousTypeId));
		storage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_TYPE_USAGE), anonymousTypeId, 42));

		StorageTransformationAnonymousTypedef::transform(storage);

		Id sourceNodeId = 0;
		for (const StorageEdge& edge : storage->getStorageEdges())
		{
			if (edge.targetNodeId == 42)
			{
				sourceNodeId = edge.sourceNodeId;
				break;
			}
		}

		std::string sourceNodeName = "";
		for (const StorageNode& node : storage->getStorageNodes())
		{
			if (node.id == sourceNodeId)
			{
				sourceNodeName = NameHierarchy::deserialize(node.serializedName).getQualifiedNameWithSignature();
				break;
			}
		}

		TS_ASSERT_EQUALS(sourceNodeName, "ClassTypedef");
	}

	void test_transformation_renames_child_node_of_anonymous_class()
	{
		std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();

		Id anonymousTypeId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_CLASS), NameHierarchy::serialize(NameHierarchy("anonymous class (input.cc<1:9>)", NAME_DELIMITER_CXX))));
		Id typedefId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_TYPEDEF), NameHierarchy::serialize(NameHierarchy("ClassTypedef", NAME_DELIMITER_CXX))));
		storage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_TYPE_USAGE), typedefId, anonymousTypeId));

		Id anonymousTypeMemberId = storage->addNode(StorageNodeData(
			Node::typeToInt(Node::NODE_FIELD),
			NameHierarchy::serialize(NameHierarchy(utility::createVectorFromElements<std::string>("anonymous class (input.cc<1:9>)", "field"), NAME_DELIMITER_CXX))
		));

		StorageTransformationAnonymousTypedef::transform(storage);

		std::string memberNodeName = "";
		for (const StorageNode& node : storage->getStorageNodes())
		{
			if (node.id == anonymousTypeMemberId)
			{
				memberNodeName = NameHierarchy::deserialize(node.serializedName).getQualifiedNameWithSignature();
				break;
			}
		}

		TS_ASSERT_EQUALS(memberNodeName, "ClassTypedef::field");
	}

	void test_transformation_does_not_rename_named_class_with_typedef_in_anonymous_namespace()
	{
		std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();

		Id typeId = storage->addNode(StorageNodeData(
			Node::typeToInt(Node::NODE_CLASS),
			NameHierarchy::serialize(NameHierarchy(utility::createVectorFromElements<std::string>("anonymous namespace (input.cc<1:9>)", "Type"), NAME_DELIMITER_CXX))
		));
		Id typedefId = storage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_TYPEDEF), NameHierarchy::serialize(NameHierarchy("ClassTypedef", NAME_DELIMITER_CXX))));
		storage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_TYPE_USAGE), typedefId, typeId));

		StorageTransformationAnonymousTypedef::transform(storage);

		const std::vector<StorageNode> nodes = storage->getStorageNodes();
		std::vector<std::string> nodeNames(nodes.size());
		std::transform(nodes.begin(), nodes.end(), nodeNames.begin(), [](const StorageNode& node)
		{
			return NameHierarchy::deserialize(node.serializedName).getQualifiedNameWithSignature();
		});

		TS_ASSERT_EQUALS(2, nodeNames.size());
		TS_ASSERT(utility::containsElement<std::string>(
			nodeNames, "ClassTypedef"
		));
		TS_ASSERT(utility::containsElement<std::string>(
			nodeNames, "anonymous namespace (input.cc<1:9>)::Type"
		));
	}
};
