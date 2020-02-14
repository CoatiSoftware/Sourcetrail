#include "ParserClientImpl.h"

#include "Edge.h"
#include "Node.h"
#include "ParseLocation.h"

ParserClientImpl::ParserClientImpl(IntermediateStorage* const storage): m_storage(storage) {}

Id ParserClientImpl::recordFile(const FilePath& filePath, bool indexed)
{
	Id fileId = addFileName(filePath);
	m_storage->addFile(StorageFile(fileId, filePath.wstr(), L"", "", indexed, true));
	return fileId;
}

void ParserClientImpl::recordFileLanguage(Id fileId, const std::wstring& languageIdentifier)
{
	m_storage->setFileLanguage(fileId, languageIdentifier);
}

Id ParserClientImpl::recordSymbol(const NameHierarchy& symbolName)
{
	return addNodeHierarchy(symbolName);
}

void ParserClientImpl::recordSymbolKind(Id symbolId, SymbolKind symbolKind)
{
	m_storage->setNodeType(symbolId, nodeKindToInt(symbolKindToNodeKind(symbolKind)));
}

void ParserClientImpl::recordAccessKind(Id symbolId, AccessKind accessKind)
{
	if (accessKind != ACCESS_NONE)
	{
		m_storage->addComponentAccess(StorageComponentAccess(symbolId, accessKindToInt(accessKind)));
	}
}

void ParserClientImpl::recordDefinitionKind(Id symbolId, DefinitionKind definitionKind)
{
	if (definitionKind != DEFINITION_NONE)
	{
		m_storage->addSymbol(StorageSymbol(symbolId, definitionKindToInt(definitionKind)));
	}
}

Id ParserClientImpl::recordReference(
	ReferenceKind referenceKind, Id referencedSymbolId, Id contextSymbolId, const ParseLocation& location)
{
	Id edgeId = addEdge(referenceKindToEdgeType(referenceKind), contextSymbolId, referencedSymbolId);
	if (edgeId)
	{
		addSourceLocation(edgeId, location, LOCATION_TOKEN);
	}
	return edgeId;
}

void ParserClientImpl::recordLocalSymbol(const std::wstring& name, const ParseLocation& location)
{
	const Id localSymbolId = m_storage->addLocalSymbol(name);
	addSourceLocation(localSymbolId, location, LOCATION_LOCAL_SYMBOL);
}

void ParserClientImpl::recordLocation(Id elementId, const ParseLocation& location, ParseLocationType type)
{
	addSourceLocation(elementId, location, parseLocationTypeToLocationType(type));
}

void ParserClientImpl::recordComment(const ParseLocation& location)
{
	if (!location.isValid())
	{
		return;
	}

	m_storage->addSourceLocation(StorageSourceLocationData(
		location.fileId,
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber,
		locationTypeToInt(LOCATION_COMMENT)));
}

void ParserClientImpl::recordError(
	const std::wstring& message,
	bool fatal,
	bool indexed,
	const FilePath& translationUnit,
	const ParseLocation& location)
{
	if (location.fileId != 0)
	{
		Id errorId = m_storage->addError(
			StorageErrorData(message, translationUnit.wstr(), fatal, indexed));

		addSourceLocation(errorId, location, LOCATION_ERROR);
	}
}

bool ParserClientImpl::hasContent() const
{
	return m_storage->getByteSize(1) > 0;
}

NodeKind ParserClientImpl::symbolKindToNodeKind(SymbolKind symbolKind) const
{
	switch (symbolKind)
	{
	case SYMBOL_ANNOTATION:
		return NODE_ANNOTATION;
	case SYMBOL_BUILTIN_TYPE:
		return NODE_BUILTIN_TYPE;
	case SYMBOL_CLASS:
		return NODE_CLASS;
	case SYMBOL_ENUM:
		return NODE_ENUM;
	case SYMBOL_ENUM_CONSTANT:
		return NODE_ENUM_CONSTANT;
	case SYMBOL_FIELD:
		return NODE_FIELD;
	case SYMBOL_FUNCTION:
		return NODE_FUNCTION;
	case SYMBOL_GLOBAL_VARIABLE:
		return NODE_GLOBAL_VARIABLE;
	case SYMBOL_INTERFACE:
		return NODE_INTERFACE;
	case SYMBOL_MACRO:
		return NODE_MACRO;
	case SYMBOL_METHOD:
		return NODE_METHOD;
	case SYMBOL_MODULE:
		return NODE_MODULE;
	case SYMBOL_NAMESPACE:
		return NODE_NAMESPACE;
	case SYMBOL_PACKAGE:
		return NODE_PACKAGE;
	case SYMBOL_STRUCT:
		return NODE_STRUCT;
	case SYMBOL_TYPEDEF:
		return NODE_TYPEDEF;
	case SYMBOL_TYPE_PARAMETER:
		return NODE_TYPE_PARAMETER;
	case SYMBOL_UNION:
		return NODE_UNION;
	default:
		break;
	}
	return NODE_SYMBOL;
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
	case REFERENCE_TYPE_ARGUMENT:
		return Edge::EDGE_TYPE_ARGUMENT;
	case REFERENCE_TEMPLATE_SPECIALIZATION:
		return Edge::EDGE_TEMPLATE_SPECIALIZATION;
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

LocationType ParserClientImpl::parseLocationTypeToLocationType(ParseLocationType type) const
{
	switch (type)
	{
	case ParseLocationType::TOKEN:
		return LOCATION_TOKEN;
	case ParseLocationType::SCOPE:
		return LOCATION_SCOPE;
	case ParseLocationType::SIGNATURE:
		return LOCATION_SIGNATURE;
	case ParseLocationType::QUALIFIER:
		return LOCATION_QUALIFIER;
	case ParseLocationType::LOCAL:
		return LOCATION_LOCAL_SYMBOL;
	}
	return LOCATION_TOKEN;
}

Id ParserClientImpl::addNodeHierarchy(const NameHierarchy& nameHierarchy)
{
	Id childNodeId = 0;
	Id firstNodeId = 0;
	for (size_t i = nameHierarchy.size(); i > 0; i--)
	{
		std::pair<Id, bool> ret = m_storage->addNode(StorageNodeData(
			nodeKindToInt(NODE_SYMBOL), NameHierarchy::serializeRange(nameHierarchy, 0, i)));

		if (!firstNodeId)
		{
			firstNodeId = ret.first;
		}

		if (childNodeId != 0)
		{
			addEdge(Edge::EDGE_MEMBER, ret.first, childNodeId);
		}

		if (!ret.second)
		{
			return firstNodeId;
		}

		childNodeId = ret.first;
	}
	return firstNodeId;
}

Id ParserClientImpl::addFileName(const FilePath& filePath)
{
	const std::wstring file = filePath.wstr();

	auto it = m_fileIdMap.find(file);
	if (it != m_fileIdMap.end())
	{
		return it->second;
	}

	const Id fileId = addNodeHierarchy(NameHierarchy(file, NAME_DELIMITER_FILE));
	m_storage->setNodeType(fileId, nodeKindToInt(NODE_FILE));

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

	Id sourceLocationId = m_storage->addSourceLocation(StorageSourceLocationData(
		location.fileId,
		location.startLineNumber,
		location.startColumnNumber,
		location.endLineNumber,
		location.endColumnNumber,
		locationTypeToInt(type)));

	m_storage->addOccurrence(StorageOccurrence(elementId, sourceLocationId));
}
