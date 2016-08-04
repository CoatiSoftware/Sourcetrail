#ifndef PARSER_CLIENT_IMPL_H
#define PARSER_CLIENT_IMPL_H

#include <set>

#include "data/graph/Node.h"
#include "data/parser/ParserClient.h"
#include "data/IntermediateStorage.h"
#include "data/graph/token_component/TokenComponentAccess.h"

#include "data/DefinitionType.h"
#include "data/SqliteStorage.h"

class ParserClientImpl: public ParserClient
{
public:
	ParserClientImpl();
	virtual ~ParserClientImpl();

	void setStorage(std::shared_ptr<IntermediateStorage> storage);
	void resetStorage();

	virtual void startParsingFile();
	virtual void finishParsingFile();


	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolType,
		AccessKind access = ACCESS_NONE, bool isImplicit = false);

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolType,
		const ParseLocation& location,
		AccessKind access = ACCESS_NONE, bool isImplicit = false);

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolType,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access = ACCESS_NONE, bool isImplicit = false);

	virtual void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location);

	virtual void onError(const ParseLocation& location, const std::string& message, bool fatal, bool indexed);

	virtual void onTypedefParsed(
		const ParseLocation& location, const NameHierarchy& typedefName, AccessKind access, bool isImplicit);
	virtual void onClassParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual void onStructParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual void onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable, bool isImplicit);
	virtual void onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessKind access, bool isImplicit);
	virtual void onFunctionParsed(
		const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation, bool isImplicit);
	virtual void onMethodParsed(
		const ParseLocation& location, const NameHierarchy& method, AccessKind access, AbstractionType abstraction,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual void onNamespaceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual void onEnumParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessKind access,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual void onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy, bool isImplicit);
	virtual void onTemplateParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, bool isImplicit);
	virtual void onLocalSymbolParsed(const std::string& name, const ParseLocation& location);
	virtual void onFileParsed(const FileInfo& fileInfo);
	virtual void onMacroDefineParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation);
	virtual void onCommentParsed(const ParseLocation& location);

	virtual void onInheritanceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const NameHierarchy& baseNameHierarchy);
	virtual void onMethodOverrideParsed(
		const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider);
	virtual void onCallParsed(
		const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee);
	virtual void onUsageParsed(
		const ParseLocation& location, const NameHierarchy& userName, SymbolKind usedType, const NameHierarchy& usedName);
	virtual void onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used);

	virtual void onTemplateArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
		const NameHierarchy& templateNameHierarchy);
	virtual void onTemplateDefaultArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy,
		const NameHierarchy& templateParameterNameHierarchy);
	virtual void onTemplateSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
		const NameHierarchy& specializedFromNameHierarchy);
	virtual void onTemplateMemberFunctionSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction);

	virtual void onFileIncludeParsed(
		const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo);

	virtual void onMacroExpandParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy);

private:
	Node::NodeType symbolKindToNodeType(SymbolKind symbolType) const;
	Edge::EdgeType referenceKindToEdgeType(ReferenceKind referenceKind) const;
	void addAccess(Id nodeId, AccessKind access);
	Id addNodeHierarchy(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType);

	Id addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime);
	Id addFile(const std::string& filePath);
	Id addNode(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType);
	Id addEdge(int type, Id sourceId, Id targetId);
	Id addLocalSymbol(const std::string& name);
	void addSourceLocation(Id elementId, const ParseLocation& location, int type);
	void addComponentAccess(Id nodeId , int type);
	void addCommentLocation(const ParseLocation& location);
	void addError(const std::string& message, bool fatal, bool indexed, const ParseLocation& location);

	void log(std::string type, std::string str, const ParseLocation& location) const;

	std::shared_ptr<IntermediateStorage> m_storage;
};

#endif // PARSER_CLIENT_IMPL_H
