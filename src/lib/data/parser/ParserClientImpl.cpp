#include "data/parser/ParserClientImpl.h"

#include "data/parser/ParseLocation.h"
#include "data/graph/Node.h"
#include "data/graph/Edge.h"
#include "utility/logging/logging.h"
#include "utility/utility.h"
#include "data/location/TokenLocation.h"

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

void ParserClientImpl::startParsing()
{
}

void ParserClientImpl::finishParsing()
{
}

void ParserClientImpl::startParsingFile(const FilePath& filePath)
{
}

void ParserClientImpl::finishParsingFile(const FilePath& filePath)
{
}

void ParserClientImpl::onError(const ParseLocation& location, const std::string& message, bool fatal)
{
	log(std::string(fatal ? "FATAL: " : "ERROR: "), message, location);

	if (!location.isValid())
	{
		return;
	}

	addError(message, fatal, location);
}

Id ParserClientImpl::onTypedefParsed(
	const ParseLocation& location, const NameHierarchy& typedefName, AccessType access, bool isImplicit)
{
	log("typedef", typedefName.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_TYPEDEF, typedefName, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addAccess(nodeId, access);

	return 0;
}

Id ParserClientImpl::onClassParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("class", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(
		Node::NODE_CLASS,
		nameHierarchy,
		(isImplicit ? DEFINITION_IMPLICIT : (scopeLocation.isValid() ? DEFINITION_EXPLICIT : DEFINITION_NONE))
	);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);

	return 0;
}

Id ParserClientImpl::onStructParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("struct", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(
		Node::NODE_STRUCT,
		nameHierarchy,
		(isImplicit ? DEFINITION_IMPLICIT : (scopeLocation.isValid() ? DEFINITION_EXPLICIT : DEFINITION_NONE))
	);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);

	return 0;
}

Id ParserClientImpl::onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable, bool isImplicit)
{
	log("global", variable.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_GLOBAL_VARIABLE, variable, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));

	return 0;
}

Id ParserClientImpl::onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessType access, bool isImplicit)
{
	log("field", field.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_FIELD, field, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addAccess(nodeId, access);

	return 0;
}

Id ParserClientImpl::onFunctionParsed(
	const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation, bool isImplicit)
{
	log("function", function.getQualifiedNameWithSignature(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_FUNCTION, function, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));

	return 0;
}

Id ParserClientImpl::onMethodParsed(
	const ParseLocation& location, const NameHierarchy& method, AccessType access, AbstractionType abstraction,
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("method", method.getQualifiedNameWithSignature(), location);

	Id nodeId = addNodeHierarchy(
		Node::NODE_METHOD,
		method,
		(isImplicit ? DEFINITION_IMPLICIT : ((location.isValid() && scopeLocation.isValid()) ? (DEFINITION_EXPLICIT) : DEFINITION_NONE))
	);
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);

	return 0;
}

Id ParserClientImpl::onNamespaceParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, const ParseLocation& scopeLocation, bool isImplicit)
{
	log("namespace", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_NAMESPACE, nameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));

	return 0;
}

Id ParserClientImpl::onEnumParsed(
	const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
	const ParseLocation& scopeLocation, bool isImplicit)
{
	log("enum", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_ENUM, nameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(nodeId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	addAccess(nodeId, access);

	return 0;
}

Id ParserClientImpl::onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy, bool isImplicit)
{
	log("enum constant", nameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_ENUM_CONSTANT, nameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));

	return 0;
}

Id ParserClientImpl::onTemplateParameterTypeParsed(
	const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, bool isImplicit)
{
	log("template parameter type", templateParameterTypeNameHierarchy.getQualifiedName(), location);

	Id nodeId = addNodeHierarchy(Node::NODE_TEMPLATE_PARAMETER_TYPE, templateParameterTypeNameHierarchy, (isImplicit ? DEFINITION_IMPLICIT : DEFINITION_EXPLICIT));
	addSourceLocation(nodeId, location, locationTypeToInt(LOCATION_TOKEN));
	addAccess(nodeId, TokenComponentAccess::ACCESS_TEMPLATE);

	return 0;
}

Id ParserClientImpl::onInheritanceParsed(
	const ParseLocation& location, const NameHierarchy& childNameHierarchy,
	const NameHierarchy& parentNameHierarchy, AccessType access)
{
	log("inheritance", childNameHierarchy.getQualifiedName() + " : " + parentNameHierarchy.getQualifiedName(), location);

	Id childNodeId = addNodeHierarchy(Node::NODE_TYPE, childNameHierarchy, DEFINITION_NONE);
	Id parentNodeId = addNodeHierarchy(Node::NODE_TYPE, parentNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_INHERITANCE, childNodeId, parentNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onMethodOverrideParsed(
	const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider)
{
	log("override", overridden.getQualifiedNameWithSignature() + " -> " + overrider.getQualifiedNameWithSignature(), location);

	Id overriddenNodeId = addNodeHierarchy(Node::NODE_FUNCTION, overridden, DEFINITION_NONE);
	Id overriderNodeId = addNodeHierarchy(Node::NODE_FUNCTION, overrider, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_OVERRIDE, overriderNodeId, overriddenNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onCallParsed(const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee)
{
	log("call", caller.getQualifiedNameWithSignature() + " -> " + callee.getQualifiedNameWithSignature(), location);

	Id callerNodeId = addNodeHierarchy(Node::NODE_FUNCTION, caller, DEFINITION_NONE);
	Id calleeNodeId = addNodeHierarchy(Node::NODE_FUNCTION, callee, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_CALL, callerNodeId, calleeNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onUsageParsed(
	const ParseLocation& location, const NameHierarchy& userName, int usedType, const NameHierarchy& usedName)
{
	log("usage", userName.getQualifiedNameWithSignature() + " -> " + usedName.getQualifiedName(), location);

	Id userNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, userName, DEFINITION_NONE);
	Id usedNodeId = addNodeHierarchy(Node::intToType(usedType), usedName, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_USAGE, userNodeId, usedNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used)
{
	log("type usage", user.getQualifiedNameWithSignature() + " -> " + used.getQualifiedName(), location);

	if (!location.isValid())
	{
		return 0;
	}

	Id functionNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, user, DEFINITION_NONE);
	Id typeNodeId = addNodeHierarchy(Node::NODE_TYPE, used, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TYPE_USAGE, functionNodeId, typeNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onTemplateArgumentTypeParsed(
	const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
	const NameHierarchy& templateNameHierarchy)
{
	log(
		"template argument type",
		argumentTypeNameHierarchy.getQualifiedName() + " -> " + templateNameHierarchy.getQualifiedName(),
		location
		);

	Id argumentNodeId = addNodeHierarchy(Node::NODE_TYPE, argumentTypeNameHierarchy, DEFINITION_NONE);
	Id templateNodeId = addNodeHierarchy(Node::NODE_UNDEFINED, templateNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_ARGUMENT, templateNodeId, argumentNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return argumentNodeId;
}

Id ParserClientImpl::onTemplateDefaultArgumentTypeParsed(
	const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy,
	const NameHierarchy& templateParameterNameHierarchy)
{
	log(
		"template default argument",
		defaultArgumentTypeNameHierarchy.getQualifiedNameWithSignature() + " -> " + templateParameterNameHierarchy.getQualifiedName(),
		location
		);

	Id defaultArgumentNodeId = addNodeHierarchy(Node::NODE_TYPE, defaultArgumentTypeNameHierarchy, DEFINITION_NONE);
	Id parameterNodeId = addNodeHierarchy(Node::NODE_TYPE, templateParameterNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_DEFAULT_ARGUMENT, parameterNodeId, defaultArgumentNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return defaultArgumentNodeId;
}

Id ParserClientImpl::onTemplateSpecializationParsed(
	const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
	const NameHierarchy& specializedFromNameHierarchy)
{
	log(
		"template record specialization",
		specializedNameHierarchy.getQualifiedName() + " -> " + specializedFromNameHierarchy.getQualifiedName(),
		location
		);

	Id specializedId = addNodeHierarchy(Node::NODE_TYPE, specializedNameHierarchy, DEFINITION_NONE);
	Id recordNodeId = addNodeHierarchy(Node::NODE_TYPE, specializedFromNameHierarchy, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_SPECIALIZATION_OF, specializedId, recordNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onTemplateMemberFunctionSpecializationParsed(
	const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction)
{
	log(
		"template member function specialization",
		instantiatedFunction.getQualifiedNameWithSignature() + " -> " + specializedFunction.getQualifiedNameWithSignature(),
		location
	);

	Id instantiatedFunctionNodeId = addNodeHierarchy(Node::NODE_FUNCTION, instantiatedFunction, DEFINITION_NONE);
	Id specializedFunctionNodeId = addNodeHierarchy(Node::NODE_FUNCTION, specializedFunction, DEFINITION_NONE);
	Id edgeId = addEdge(Edge::EDGE_TEMPLATE_MEMBER_SPECIALIZATION_OF, instantiatedFunctionNodeId, specializedFunctionNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onLocalSymbolParsed(const std::string& name, const ParseLocation& location)
{
	log(
		"local symbol",
		name,
		location
	);

	Id localSymbolId = addLocalSymbol(name);
	addSourceLocation(localSymbolId, location, locationTypeToInt(LOCATION_LOCAL_SYMBOL));

	return localSymbolId;
}

Id ParserClientImpl::onFileParsed(const FileInfo& fileInfo) // TODO: move up to nodes
{
	log("file", fileInfo.path.str(), ParseLocation());

	addFile(fileInfo.path.fileName(), fileInfo.path.str(), utility::timeToString(fileInfo.lastWriteTime));

	return 0;
}

Id ParserClientImpl::onFileIncludeParsed(const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo)
{
	log("include", includedFileInfo.path.str(), location);

	Id fileNodeId = addFile(fileInfo.path.fileName(), fileInfo.path.str(), utility::timeToString(fileInfo.lastWriteTime));
	Id includedFileNodeId = addFile(includedFileInfo.path.fileName(), includedFileInfo.path.str(), utility::timeToString(includedFileInfo.lastWriteTime));
	Id edgeId = addEdge(Edge::EDGE_INCLUDE, fileNodeId, includedFileNodeId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return fileNodeId;
}

Id ParserClientImpl::onMacroDefineParsed(
	const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation)
{
	log("macro", macroNameHierarchy.getQualifiedName(), location);

	Id macroId = addNodeHierarchy(Node::NODE_MACRO, macroNameHierarchy, DEFINITION_EXPLICIT);
	addSourceLocation(macroId, location, locationTypeToInt(LOCATION_TOKEN));
	addSourceLocation(macroId, scopeLocation, locationTypeToInt(LOCATION_SCOPE));
	//Id fileNodeId = getFileNodeId(location.filePath); // do we need this???
	//addEdge(Edge::EDGE_MACRO_USAGE, fileNodeId, macroId, , location);

	return macroId;
}

Id ParserClientImpl::onMacroExpandParsed(const ParseLocation &location, const NameHierarchy& macroNameHierarchy)
{
	log("macro use", macroNameHierarchy.getQualifiedName(), location);

	Id macroExpandId = addNodeHierarchy(Node::NODE_MACRO, macroNameHierarchy, DEFINITION_NONE);
	Id fileNodeId = addFile(location.filePath.str());
	Id edgeId = addEdge(Edge::EDGE_MACRO_USAGE, fileNodeId, macroExpandId);
	addSourceLocation(edgeId, location, locationTypeToInt(LOCATION_TOKEN));

	return edgeId;
}

Id ParserClientImpl::onCommentParsed(const ParseLocation& location) // TODO: move up to nodes
{
	log("comment", "no name", location);

	addFile(location.filePath.str());
	addCommentLocation(location);

	return 0;
}

TokenComponentAccess::AccessType ParserClientImpl::convertAccessType(ParserClient::AccessType access) const
{
	switch (access)
	{
	case ACCESS_PUBLIC:
		return TokenComponentAccess::ACCESS_PUBLIC;
	case ACCESS_PROTECTED:
		return TokenComponentAccess::ACCESS_PROTECTED;
	case ACCESS_PRIVATE:
		return TokenComponentAccess::ACCESS_PRIVATE;
	case ACCESS_NONE:
		return TokenComponentAccess::ACCESS_NONE;
	}
}

void ParserClientImpl::addAccess(Id nodeId, ParserClient::AccessType access)
{
	addAccess(nodeId, convertAccessType(access));
}

void ParserClientImpl::addAccess(Id nodeId, TokenComponentAccess::AccessType access)
{
	if (access == TokenComponentAccess::ACCESS_NONE)
	{
		return;
	}

	addComponentAccess(nodeId, access);
}

Id ParserClientImpl::addNodeHierarchy(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType)
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
		Node::NodeType currentType = (currentIsLastElement ? nodeType : Node::NODE_UNDEFINED); // TODO: rename to unknown!
		DefinitionType currentDefinitionType = (currentIsLastElement ? definitionType : DEFINITION_NONE);

		Id nodeId = addNode(currentType, currentNameHierarchy, currentDefinitionType);

		// Todo: performance optimization: check if node exists. dont add edge if it existed before...
		if (parentNodeId != 0)
		{
			addEdge(Edge::EDGE_MEMBER, parentNodeId, nodeId);
		}

		parentNodeId = nodeId;
	}
	return parentNodeId;
}




Id ParserClientImpl::addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addFile(name, filePath, modificationTime);
}

Id ParserClientImpl::addFile(const std::string& filePath)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addFile(filePath);
}

Id ParserClientImpl::addNode(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType)
{
	if (!m_storage)
	{
		return 0;
	}

	return m_storage->addNode(Node::typeToInt(nodeType), nameHierarchy, definitionTypeToInt(definitionType));
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

	m_storage->addSourceLocation(elementId, location, type);
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

	m_storage->addCommentLocation(location);
}

void ParserClientImpl::addError(const std::string& message, bool fatal, const ParseLocation& location)
{
	if (!m_storage)
	{
		return;
	}

	m_storage->addError(message, fatal, location);
}

void ParserClientImpl::log(std::string type, std::string str, const ParseLocation& location) const
{
	LOG_INFO_STREAM_BARE(
		<< type << ": " << str << " <" << location.filePath.str() << " "
		<< location.startLineNumber << ":" << location.startColumnNumber << " "
		<< location.endLineNumber << ":" << location.endColumnNumber << ">"
	);
}
