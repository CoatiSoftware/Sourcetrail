#ifndef PARSER_CLIENT_IMPL_H
#define PARSER_CLIENT_IMPL_H

#include "data/parser/ParserClient.h"
#include "data/IntermediateStorage.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/Node.h"

#include "data/DefinitionType.h"
#include "data/SqliteStorage.h"

class ParserClientImpl: public ParserClient
{
public:
	ParserClientImpl();
	virtual ~ParserClientImpl();

	void setStorage(std::shared_ptr<IntermediateStorage> storage);
	void resetStorage();

	virtual void startParsing();
	virtual void finishParsing();

	virtual void startParsingFile(const FilePath& filePath);
	virtual void finishParsingFile(const FilePath& filePath);

	virtual void onError(const ParseLocation& location, const std::string& message, bool fatal);

	virtual Id onTypedefParsed(
		const ParseLocation& location, const NameHierarchy& typedefName, AccessType access, bool isImplicit);
	virtual Id onClassParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual Id onStructParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual Id onGlobalVariableParsed(const ParseLocation& location, const NameHierarchy& variable, bool isImplicit);
	virtual Id onFieldParsed(const ParseLocation& location, const NameHierarchy& field, AccessType access, bool isImplicit);
	virtual Id onFunctionParsed(
		const ParseLocation& location, const NameHierarchy& function, const ParseLocation& scopeLocation, bool isImplicit);
	virtual Id onMethodParsed(
		const ParseLocation& location, const NameHierarchy& method, AccessType access, AbstractionType abstraction,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual Id onNamespaceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual Id onEnumParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy, AccessType access,
		const ParseLocation& scopeLocation, bool isImplicit);
	virtual Id onEnumConstantParsed(const ParseLocation& location, const NameHierarchy& nameHierarchy, bool isImplicit);
	virtual Id onTemplateParameterTypeParsed(
		const ParseLocation& location, const NameHierarchy& templateParameterTypeNameHierarchy, bool isImplicit);

	virtual Id onInheritanceParsed(
		const ParseLocation& location, const NameHierarchy& nameHierarchy,
		const NameHierarchy& baseNameHierarchy, AccessType access);
	virtual Id onMethodOverrideParsed(
		const ParseLocation& location, const NameHierarchy& overridden, const NameHierarchy& overrider);
	virtual Id onCallParsed(
		const ParseLocation& location, const NameHierarchy& caller, const NameHierarchy& callee);
	virtual Id onFieldUsageParsed(
		const ParseLocation& location, const NameHierarchy& userNameHierarchy, const NameHierarchy& usedNameHierarchy);
	virtual Id onGlobalVariableUsageParsed(
		const ParseLocation& location, const NameHierarchy& userNameHierarchy, const NameHierarchy& usedNameHierarchy);
	virtual Id onEnumConstantUsageParsed(
		const ParseLocation& location, const NameHierarchy& userNameHierarchy, const NameHierarchy& usedNameHierarchy);
	virtual Id onTypeUsageParsed(const ParseLocation& location, const NameHierarchy& user, const NameHierarchy& used);

	virtual Id onTemplateArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& argumentTypeNameHierarchy,
		const NameHierarchy& templateNameHierarchy);
	virtual Id onTemplateDefaultArgumentTypeParsed(
		const ParseLocation& location, const NameHierarchy& defaultArgumentTypeNameHierarchy,
		const NameHierarchy& templateArgumentTypeNameHierarchy);
	virtual Id onTemplateSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& specializedNameHierarchy,
		const NameHierarchy& specializedFromNameHierarchy);
	virtual Id onTemplateMemberFunctionSpecializationParsed(
		const ParseLocation& location, const NameHierarchy& instantiatedFunction, const NameHierarchy& specializedFunction);

	virtual Id onFileParsed(const FileInfo& fileInfo);
	virtual Id onFileIncludeParsed(
		const ParseLocation& location, const FileInfo& fileInfo, const FileInfo& includedFileInfo);

	virtual Id onMacroDefineParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy, const ParseLocation& scopeLocation);
	virtual Id onMacroExpandParsed(
		const ParseLocation& location, const NameHierarchy& macroNameHierarchy);

	virtual Id onCommentParsed(const ParseLocation& location);

private:
	TokenComponentAccess::AccessType convertAccessType(ParserClient::AccessType access) const;
	void addAccess(Id nodeId, ParserClient::AccessType access);
	void addAccess(Id nodeId, TokenComponentAccess::AccessType access);
	Id addNodeHierarchy(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType);

	Id addFile(const std::string& name, const std::string& filePath, const std::string& modificationTime);
	Id addFile(const std::string& filePath);
	Id addNode(Node::NodeType nodeType, NameHierarchy nameHierarchy, DefinitionType definitionType);
	Id addEdge(int type, Id sourceId, Id targetId);
	void addSourceLocation(Id elementId, const ParseLocation& location, bool isScope);
	void addComponentAccess(Id nodeId , int type);
	void addCommentLocation(const ParseLocation& location);
	void addError(const std::string& message, bool fatal, const ParseLocation& location);

	void log(std::string type, std::string str, const ParseLocation& location) const;

	std::shared_ptr<IntermediateStorage> m_storage;
};

#endif // PARSER_CLIENT_IMPL_H
