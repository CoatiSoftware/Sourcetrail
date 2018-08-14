#include "data/parser/ParserClientImpl.h"

#include "data/graph/Edge.h"
#include "data/graph/Node.h"
#include "data/location/LocationType.h"
#include "data/parser/ParseLocation.h"
#include "utility/logging/logging.h"

ParserClientImpl::ParserClientImpl()
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

Id ParserClientImpl::recordSymbolWithLocation(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = recordSymbol(symbolName, symbolKind, access, definitionKind);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	return nodeId;
}

Id ParserClientImpl::recordSymbolWithLocationAndScope(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location, const ParseLocation& scopeLocation,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = recordSymbolWithLocation(symbolName, symbolKind, location, access, definitionKind);
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	return nodeId;
}

Id ParserClientImpl::recordSymbolWithLocationAndScopeAndSignature(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location, const ParseLocation& scopeLocation, const ParseLocation& signatureLocation,
	AccessKind access, DefinitionKind definitionKind)
{
	Id nodeId = recordSymbolWithLocationAndScope(symbolName, symbolKind, location, scopeLocation, access, definitionKind);
	addSourceLocation(nodeId, signatureLocation, locationTypeToInt(LOCATION_SIGNATURE));
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

void ParserClientImpl::recordQualifierLocation(const NameHierarchy& qualifierName, const ParseLocation& location)
{
	Id nodeId = addNodeHierarchy(qualifierName, NodeType::NODE_SYMBOL);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_QUALIFIER));
}

void ParserClientImpl::recordLocalSymbol(const std::wstring& name, const ParseLocation& location)
{
	const Id localSymbolId = addLocalSymbol(name);
	addSourceLocation(localSymbolId, location, locationTypeToInt(LOCATION_LOCAL_SYMBOL));
}

void ParserClientImpl::recordFile(const FileInfo& fileInfo, bool indexed)
{
	const Id nodeId = addNodeHierarchy(NameHierarchy(fileInfo.path.wstr(), NAME_DELIMITER_FILE), NodeType::NODE_FILE);
	addFile(nodeId, fileInfo.path, fileInfo.lastWriteTime.toString(), indexed);
}

void ParserClientImpl::recordComment(const ParseLocation& location)
{
	addCommentLocation(location);
}

void ParserClientImpl::doRecordError(
	const ParseLocation& location, const std::wstring& message, bool fatal, bool indexed, const FilePath& translationUnit)
{
	if (location.isValid())
	{
		addError(message, fatal, indexed, location, translationUnit);
	}
}

NodeType ParserClientImpl::symbolKindToNodeType(SymbolKind symbolKind) const
{
	switch (symbolKind)
	{
	case SYMBOL_ANNOTATION:
		return NodeType::NODE_ANNOTATION;
	case SYMBOL_BUILTIN_TYPE:
		return NodeType::NODE_BUILTIN_TYPE;
	case SYMBOL_CLASS:
		return NodeType::NODE_CLASS;
	case SYMBOL_ENUM:
		return NodeType::NODE_ENUM;
	case SYMBOL_ENUM_CONSTANT:
		return NodeType::NODE_ENUM_CONSTANT;
	case SYMBOL_FIELD:
		return NodeType::NODE_FIELD;
	case SYMBOL_FUNCTION:
		return NodeType::NODE_FUNCTION;
	case SYMBOL_GLOBAL_VARIABLE:
		return NodeType::NODE_GLOBAL_VARIABLE;
	case SYMBOL_INTERFACE:
		return NodeType::NODE_INTERFACE;
	case SYMBOL_MACRO:
		return NodeType::NODE_MACRO;
	case SYMBOL_METHOD:
		return NodeType::NODE_METHOD;
	case SYMBOL_NAMESPACE:
		return NodeType::NODE_NAMESPACE;
	case SYMBOL_PACKAGE:
		return NodeType::NODE_PACKAGE;
	case SYMBOL_STRUCT:
		return NodeType::NODE_STRUCT;
	case SYMBOL_TEMPLATE_PARAMETER:
		return NodeType::NODE_TEMPLATE_PARAMETER_TYPE;
	case SYMBOL_TYPEDEF:
		return NodeType::NODE_TYPEDEF;
	case SYMBOL_TYPE_PARAMETER:
		return NodeType::NODE_TYPE_PARAMETER;
	case SYMBOL_UNION:
		return NodeType::NODE_UNION;
	default:
		break;
	}
	return NodeType::NODE_SYMBOL;
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
	case REFERENCE_ANNOTATION_USAGE:
		return Edge::EDGE_ANNOTATION_USAGE;
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

Id ParserClientImpl::addNodeHierarchy(const NameHierarchy& nameHierarchy, NodeType nodeType)
{
	if (nameHierarchy.size() == 0)
	{
		return 0;
	}

	Id parentNodeId = 0;
	NameHierarchy currentNameHierarchy(nameHierarchy.getDelimiter());

	for (size_t i = 0; i < nameHierarchy.size(); i++)
	{
		currentNameHierarchy.push(nameHierarchy[i]);
		const bool currentIsLastElement = (i == nameHierarchy.size() - 1);
		const NodeType currentType = (currentIsLastElement ? nodeType : NodeType::NODE_SYMBOL); // TODO: rename to unknown!

		Id nodeId = addNode(currentType, currentNameHierarchy);

		if (parentNodeId != 0)
		{
			addEdge(Edge::EDGE_MEMBER, parentNodeId, nodeId);
		}

		parentNodeId = nodeId;
	}
	return parentNodeId;
}

Id ParserClientImpl::addNode(NodeType nodeType, const NameHierarchy& nameHierarchy)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addNode(StorageNodeData(NodeType::typeToInt(nodeType.getType()), NameHierarchy::serialize(nameHierarchy)));
}

void ParserClientImpl::addFile(Id id, const FilePath& filePath, const std::string& modificationTime, bool indexed)
{
	if (m_storage)
	{
		m_storage->addFile(StorageFile(id, filePath.wstr(), modificationTime, indexed, true));
	}
}

void ParserClientImpl::addSymbol(Id id, DefinitionKind definitionKind)
{
	if (!m_storage)
	{
		return;
	}

	if (definitionKind != DEFINITION_NONE)
	{
		m_storage->addSymbol(StorageSymbol(id, definitionKindToInt(definitionKind)));
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

	return m_storage->addEdge(StorageEdgeData(type, sourceId, targetId));
}

Id ParserClientImpl::addLocalSymbol(const std::wstring& name)
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

	Id sourceLocationId = m_storage->addSourceLocation(StorageSourceLocationData(
		addNodeHierarchy(NameHierarchy(location.filePath.wstr(), NAME_DELIMITER_FILE), NodeType::NODE_FILE),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber,
		type
	));

	m_storage->addOccurrence(StorageOccurrence(
		elementId,
		sourceLocationId
	));
}

void ParserClientImpl::addComponentAccess(Id nodeId , int type)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addComponentAccess(StorageComponentAccess(nodeId, type));
}

void ParserClientImpl::addCommentLocation(const ParseLocation& location)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addCommentLocation(StorageCommentLocationData(
		addNodeHierarchy(NameHierarchy(location.filePath.wstr(), NAME_DELIMITER_FILE), NodeType::NODE_FILE),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber
	));
}

void ParserClientImpl::addError(
	const std::wstring& message, bool fatal, bool indexed, const ParseLocation& location, const FilePath& translationUnit)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addError(StorageErrorData(
		message, location.filePath.wstr(), location.startLineNumber, location.startColumnNumber, translationUnit.wstr(), fatal, indexed
	));
}
