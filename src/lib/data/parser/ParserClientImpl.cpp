#include "ParserClientImpl.h"

#include "Edge.h"
#include "Node.h"
#include "ParseLocation.h"
#include "logging.h"

ParserClientImpl::ParserClientImpl(IntermediateStorage* const storage)
	: m_storage(storage)
{
}

Id ParserClientImpl::recordSymbol(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = addNodeHierarchy(symbolName, symbolKindToNodeType(symbolKind));
	if (definitionKind != DEFINITION_NONE)
	{
		m_storage->addSymbol(StorageSymbol(nodeId, definitionKindToInt(definitionKind)));
	}
	if (access != ACCESS_NONE)
	{
		m_storage->addComponentAccess(StorageComponentAccess(nodeId, accessKindToInt(access)));
	}
	return nodeId;
}

Id ParserClientImpl::recordSymbolWithLocation(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = recordSymbol(symbolName, symbolKind, access, definitionKind);
	addSourceLocation(nodeId, location, LOCATION_TOKEN);
	return nodeId;
}

Id ParserClientImpl::recordSymbolWithLocationAndScope(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location, const ParseLocation& scopeLocation,
	AccessKind access, DefinitionKind definitionKind
)
{
	Id nodeId = recordSymbolWithLocation(symbolName, symbolKind, location, access, definitionKind);
	addSourceLocation(nodeId, scopeLocation, LOCATION_SCOPE);
	return nodeId;
}

Id ParserClientImpl::recordSymbolWithLocationAndScopeAndSignature(
	const NameHierarchy& symbolName, SymbolKind symbolKind,
	const ParseLocation& location, const ParseLocation& scopeLocation, const ParseLocation& signatureLocation,
	AccessKind access, DefinitionKind definitionKind)
{
	Id nodeId = recordSymbolWithLocationAndScope(symbolName, symbolKind, location, scopeLocation, access, definitionKind);
	addSourceLocation(nodeId, signatureLocation, LOCATION_SIGNATURE);
	return nodeId;
}

void ParserClientImpl::recordReference(
	ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
	const ParseLocation& location)
{
	Id contextNodeId = addNodeHierarchy(contextName);
	Id referencedNodeId = addNodeHierarchy(referencedName);
	Id edgeId = addEdge(referenceKindToEdgeType(referenceKind), contextNodeId, referencedNodeId);
	addSourceLocation(edgeId, location, LOCATION_TOKEN);
}

void ParserClientImpl::recordQualifierLocation(const NameHierarchy& qualifierName, const ParseLocation& location)
{
	Id nodeId = addNodeHierarchy(qualifierName, NodeType::NODE_SYMBOL);
	addSourceLocation(nodeId, location, LOCATION_QUALIFIER);
}

void ParserClientImpl::recordLocalSymbol(const std::wstring& name, const ParseLocation& location)
{
	const Id localSymbolId = m_storage->addLocalSymbol(name);
	addSourceLocation(localSymbolId, location, LOCATION_LOCAL_SYMBOL);
}

void ParserClientImpl::recordFile(const FilePath& filePath, bool indexed)
{
	const Id fileId = addFileName(filePath);
	m_storage->addFile(StorageFile(fileId, filePath.wstr(), indexed, true));
}

void ParserClientImpl::recordComment(const ParseLocation& location)
{
	m_storage->addCommentLocation(StorageCommentLocationData(
		addFileName(location.filePath),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber
	));
}

void ParserClientImpl::doRecordError(
	const ParseLocation& location, const std::wstring& message, bool fatal, bool indexed, const FilePath& translationUnit)
{
	if (location.isValid())
	{
		m_storage->addError(StorageErrorData(
			message,
			location.filePath.wstr(),
			location.startLineNumber,
			location.startColumnNumber,
			translationUnit.wstr(),
			fatal,
			indexed
		));
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

Id ParserClientImpl::addNodeHierarchy(const NameHierarchy& nameHierarchy, NodeType nodeType)
{
	Id parentNodeId = 0;
	for (size_t i = 1; i <= nameHierarchy.size(); i++)
	{
		const NodeType currentType = (i == nameHierarchy.size() ? nodeType : NodeType::NODE_SYMBOL); // TODO: rename to unknown!

		Id nodeId = m_storage->addNode(StorageNodeData(
			NodeType::typeToInt(currentType.getType()), NameHierarchy::serializeRange(nameHierarchy, 0, i)));

		if (parentNodeId != 0)
		{
			addEdge(Edge::EDGE_MEMBER, parentNodeId, nodeId);
		}

		parentNodeId = nodeId;
	}
	return parentNodeId;
}

Id ParserClientImpl::addFileName(const FilePath& filePath)
{
	const std::wstring file = filePath.wstr();

	auto it = m_fileIdMap.find(file);
	if (it != m_fileIdMap.end())
	{
		return it->second;
	}

	const Id fileId = addNodeHierarchy(NameHierarchy(file, NAME_DELIMITER_FILE), NodeType::NODE_FILE);
	m_fileIdMap.emplace(file, fileId);
	return fileId;
}

Id ParserClientImpl::addEdge(int type, Id sourceId, Id targetId)
{
	if (sourceId && targetId)
	{
		return m_storage->addEdge(StorageEdgeData(type, sourceId, targetId));
	}
	return 0;
}

void ParserClientImpl::addSourceLocation(Id elementId, const ParseLocation& location, LocationType type)
{
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
		addFileName(location.filePath),
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber,
		locationTypeToInt(type)
	));

	m_storage->addOccurrence(StorageOccurrence(
		elementId,
		sourceLocationId
	));
}
