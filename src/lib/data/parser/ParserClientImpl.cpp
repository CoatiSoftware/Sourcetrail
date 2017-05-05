#include "data/parser/ParserClientImpl.h"

#include "data/graph/Edge.h"
#include "data/graph/Node.h"
#include "data/location/LocationType.h"
#include "data/parser/ParseLocation.h"
#include "utility/logging/logging.h"
#include "utility/utility.h"

ParserClientImpl::ParserClientImpl()
{
}

ParserClientImpl::~ParserClientImpl()
{
}

void ParserClientImpl::setStorage(std::shared_ptr<IntermediateStorage> storage)
{
	m_storage = storage;
}

void ParserClientImpl::resetStorage()
{
	m_storage.reset();
}

Id ParserClientImpl::recordSymbol(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = addNodeHierarchy(symbolName, symbolKindToNodeType(symbolKind));
	addSymbol(nodeId, definitionKind);
	addAccess(nodeId, access);
	return nodeId;
}

Id ParserClientImpl::recordSymbol(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = recordSymbol(symbolName, symbolKind, access, definitionKind);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	return nodeId;
}

Id ParserClientImpl::recordSymbol(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location, const ParseLocation& scopeLocation,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = recordSymbol(symbolName, symbolKind, location, access, definitionKind);
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	return nodeId;
}

void ParserClientImpl::recordReference(
	ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
	const ParseLocation& location)
{
	Id contextNodeId = addNodeHierarchy(contextName);
	Id referencedNodeId = addNodeHierarchy(referencedName);
	Id edgeId = addEdge(referenceKindToEdgeType(referenceKind), contextNodeId, referencedNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));
}

void ParserClientImpl::onError(const ParseLocation& location, const std::string& message, bool fatal, bool indexed)
{
	if (location.isValid())
	{
		addError(message, fatal, indexed, location);
	}
}

void ParserClientImpl::onLocalSymbolParsed(const std::string& name, const ParseLocation& location)
{
	const Id localSymbolId = addLocalSymbol(name);
	addSourceLocation(localSymbolId, location, locationTypeToInt(LOCATION_LOCAL_SYMBOL));
}

void ParserClientImpl::onFileParsed(const FileInfo& fileInfo)
{
	const Id nodeId = addNodeHierarchy(NameHierarchy(fileInfo.path.str()), Node::NODE_FILE);
	addFile(nodeId, fileInfo.path, fileInfo.lastWriteTime.toString());
}

void ParserClientImpl::onCommentParsed(const ParseLocation& location)
{
	addCommentLocation(location);
}

Node::NodeType ParserClientImpl::symbolKindToNodeType(SymbolKind symbolKind) const
{
	switch (symbolKind)
	{
	case SYMBOL_BUILTIN_TYPE:
		return Node::NODE_BUILTIN_TYPE;
	case SYMBOL_CLASS:
		return Node::NODE_CLASS;
	case SYMBOL_ENUM:
		return Node::NODE_ENUM;
	case SYMBOL_ENUM_CONSTANT:
		return Node::NODE_ENUM_CONSTANT;
	case SYMBOL_FIELD:
		return Node::NODE_FIELD;
	case SYMBOL_FUNCTION:
		return Node::NODE_FUNCTION;
	case SYMBOL_GLOBAL_VARIABLE:
		return Node::NODE_GLOBAL_VARIABLE;
	case SYMBOL_INTERFACE:
		return Node::NODE_INTERFACE;
	case SYMBOL_MACRO:
		return Node::NODE_MACRO;
	case SYMBOL_METHOD:
		return Node::NODE_METHOD;
	case SYMBOL_NAMESPACE:
		return Node::NODE_NAMESPACE;
	case SYMBOL_PACKAGE:
		return Node::NODE_PACKAGE;
	case SYMBOL_STRUCT:
		return Node::NODE_STRUCT;
	case SYMBOL_TEMPLATE_PARAMETER:
		return Node::NODE_TEMPLATE_PARAMETER_TYPE;
	case SYMBOL_TYPEDEF:
		return Node::NODE_TYPEDEF;
	case SYMBOL_TYPE_PARAMETER:
		return Node::NODE_TYPE_PARAMETER;
	case SYMBOL_UNION:
		return Node::NODE_TYPE;
	default:
		break;
	}
	return Node::NODE_NON_INDEXED;
}

Edge::EdgeType ParserClientImpl::referenceKindToEdgeType(ReferenceKind referenceKind) const
{
	switch (referenceKind)
	{
	case REFERENCE_TYPE_USAGE:
		return Edge::EDGE_TYPE_USAGE;
	case REFERENCE_USAGE:
		return Edge::EDGE_USAGE;
	case REFERENCE_CALL:
		return Edge::EDGE_CALL;
	case REFERENCE_INHERITANCE:
		return Edge::EDGE_INHERITANCE;
	case REFERENCE_OVERRIDE:
		return Edge::EDGE_OVERRIDE;
	case REFERENCE_TEMPLATE_ARGUMENT:
		return Edge::EDGE_TEMPLATE_ARGUMENT;
	case REFERENCE_TYPE_ARGUMENT:
		return Edge::EDGE_TYPE_ARGUMENT;
	case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
		return Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT;
	case REFERENCE_TEMPLATE_SPECIALIZATION:
		return Edge::EDGE_TEMPLATE_SPECIALIZATION;
	case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION:
		return Edge::EDGE_TEMPLATE_MEMBER_SPECIALIZATION;
	case REFERENCE_INCLUDE:
		return Edge::EDGE_INCLUDE;
	case REFERENCE_IMPORT:
		return Edge::EDGE_IMPORT;
	case REFERENCE_MACRO_USAGE:
		return Edge::EDGE_MACRO_USAGE;
	default:
		break;
	}
	return Edge::EDGE_UNDEFINED;
}

void ParserClientImpl::addAccess(Id nodeId, AccessKind access)
{
	if (access != ACCESS_NONE)
	{
		addComponentAccess(nodeId, accessKindToInt(access));
	}
}

Id ParserClientImpl::addNodeHierarchy(NameHierarchy nameHierarchy, Node::NodeType nodeType)
{
	if (nameHierarchy.size() == 0)
	{
		return 0;
	}

	Id parentNodeId = 0;
	NameHierarchy currentNameHierarchy;

	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		currentNameHierarchy.push(nameHierarchy[i]);
		const bool currentIsLastElement = (i == nameHierarchy.size() - 1);
		const Node::NodeType currentType = (currentIsLastElement ? nodeType : Node::NODE_NON_INDEXED); // TODO: rename to unknown!

		Id nodeId = addNode(currentType, currentNameHierarchy);

		// Todo: performance optimization: check if node exists. dont add edge if it existed before...
		if (parentNodeId != 0)
		{
			addEdge(Edge::EDGE_MEMBER, parentNodeId, nodeId);
		}

		parentNodeId = nodeId;
	}
	return parentNodeId;
}

Id ParserClientImpl::addNode(Node::NodeType nodeType, NameHierarchy nameHierarchy)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addNode(Node::typeToInt(nodeType), NameHierarchy::serialize(nameHierarchy));
}

void ParserClientImpl::addFile(Id id, const FilePath& filePath, const std::string& modificationTime)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addFile(id, filePath.str(), modificationTime, true);
}

void ParserClientImpl::addSymbol(Id id, DefinitionKind definitionKind)
{
	if (!m_storage)
	{
		return;
	}

	if (definitionKind != DEFINITION_NONE)
	{
		m_storage->addSymbol(id, definitionKindToInt(definitionKind));
	}
}

Id ParserClientImpl::addEdge(int type, Id sourceId, Id targetId)
{
	if (!m_storage)
	{
		return 0;
	}

	if (!sourceId || !targetId)
	{
		return 0;
	}

	return m_storage->addEdge(type, sourceId, targetId);
}

Id ParserClientImpl::addLocalSymbol(const std::string& name)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addLocalSymbol(name);
}

void ParserClientImpl::addSourceLocation(Id elementId, const ParseLocation& location, int type)
{
	if (!m_storage)
	{
		return;
	}

	if (!location.isValid())
	{
		return;
	}

	if (location.filePath.empty())
	{
		LOG_ERROR("no filename set!");
		return;
	}

	Id sourceLocationId = m_storage->addSourceLocation(
		addNodeHierarchy(NameHierarchy(location.filePath.str()), Node::NODE_FILE),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber,
		type
	);

	m_storage->addOccurrence(
		elementId,
		sourceLocationId
	);
}

void ParserClientImpl::addComponentAccess(Id nodeId , int type)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addComponentAccess(nodeId, type);
}

void ParserClientImpl::addCommentLocation(const ParseLocation& location)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addCommentLocation(
		addNodeHierarchy(NameHierarchy(location.filePath.str()), Node::NODE_FILE),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber
	);
}

void ParserClientImpl::addError(const std::string& message, bool fatal, bool indexed, const ParseLocation& location)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addError(message, location.filePath, location.startLineNumber, location.startColumnNumber, fatal, indexed);
}
