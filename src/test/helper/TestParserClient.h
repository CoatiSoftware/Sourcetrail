#ifndef TEST_PARSER_CLIENT_H
#define TEST_PARSER_CLIENT_H

#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"

class TestParserClient: public ParserClient
{
public:
	virtual void startParsingFile()
	{
	}

	virtual void finishParsingFile()
	{
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		AccessKind access, DefinitionType definitionType)
	{
		std::vector<std::string>* bin = getBinForSymbolKind(symbolKind);
		if (bin != nullptr)
		{
			bin->push_back(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access));
		}
		return 0;
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionType definitionType)
	{
		std::vector<std::string>* bin = getBinForSymbolKind(symbolKind);
		if (bin != nullptr)
		{
			bin->push_back(addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access), location));
		}
		return 0;
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionType definitionType)
	{
		std::vector<std::string>* bin = getBinForSymbolKind(symbolKind);
		if (bin != nullptr)
		{
			bin->push_back(addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access), location, scopeLocation));
		}
		return 0;
	}

	void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location)
	{
		std::vector<std::string>* referenceContainer = nullptr;
		switch (referenceKind)
		{
		case REFERENCE_TYPE_USAGE:
			referenceContainer = &typeUses;
			break;
		case REFERENCE_USAGE:
			referenceContainer = &usages;
			break;
		case REFERENCE_CALL:
			referenceContainer = &calls;
			break;
		case REFERENCE_INHERITANCE:
			referenceContainer = &inheritances;
			break;
		case REFERENCE_OVERRIDE:
			referenceContainer = &overrides;
			break;
		case REFERENCE_TEMPLATE_ARGUMENT:
			referenceContainer = &templateArgumentTypes;
			break;
		case REFERENCE_TYPE_ARGUMENT:
			referenceContainer = &typeArguments;
			break;
		case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
			referenceContainer = &templateDefaultArgumentTypes;
			break;
		case REFERENCE_TEMPLATE_SPECIALIZATION:
			referenceContainer = &templateSpecializations;
			break;
		case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION:
			referenceContainer = &templateMemberSpecializations;
			break;
		case REFERENCE_INCLUDE:
			referenceContainer = &includes;
			break;
		case REFERENCE_IMPORT:
			referenceContainer = &imports;
			break;
		case REFERENCE_MACRO_USAGE:
			referenceContainer = &macroUses;
			break;
		default:
			break;
		}
		if (referenceContainer != nullptr)
		{
			referenceContainer->push_back(addLocationSuffix(
				contextName.getQualifiedNameWithSignature() + " -> " + referencedName.getQualifiedNameWithSignature(), location)
			);
		}
	}

	virtual void onError(const ParseLocation& location, const std::string& message, bool fatal, bool indexed)
	{
		errors.push_back(addLocationSuffix(message, location));
	}

	virtual void onLocalSymbolParsed(const std::string& name, const ParseLocation& location)
	{
		localSymbols.push_back(addLocationSuffix(name, location));
	}

	virtual void onFileParsed(const FileInfo& fileInfo)
	{
		files.insert(fileInfo.path.str());
	}

	virtual void onCommentParsed(const ParseLocation& location)
	{
		comments.push_back(addLocationSuffix("comment", location));
	}

	std::vector<std::string> errors;

	std::vector<std::string> packages;
	std::vector<std::string> typedefs;
	std::vector<std::string> classes;
	std::vector<std::string> interfaces;
	std::vector<std::string> enums;
	std::vector<std::string> enumConstants;
	std::vector<std::string> functions;
	std::vector<std::string> fields;
	std::vector<std::string> globalVariables;
	std::vector<std::string> methods;
	std::vector<std::string> namespaces;
	std::vector<std::string> structs;
	std::vector<std::string> macros;
	std::vector<std::string> templateParameterTypes;
	std::vector<std::string> typeParameters;
	std::vector<std::string> localSymbols;
	std::set<std::string> files;
	std::vector<std::string> comments;

	std::vector<std::string> inheritances;
	std::vector<std::string> overrides;
	std::vector<std::string> calls;
	std::vector<std::string> usages;	// for variables
	std::vector<std::string> typeUses;	// for types
	std::vector<std::string> macroUses;
	std::vector<std::string> templateArgumentTypes;
	std::vector<std::string> typeArguments;
	std::vector<std::string> templateDefaultArgumentTypes;
	std::vector<std::string> templateSpecializations;
	std::vector<std::string> templateMemberSpecializations;
	std::vector<std::string> includes;
	std::vector<std::string> imports;

private:
	std::vector<std::string>* getBinForSymbolKind(SymbolKind symbolType)
	{
		switch (symbolType)
		{
		case SYMBOL_PACKAGE:
			return &packages;
		case SYMBOL_TYPEDEF:
			return &typedefs;
		case SYMBOL_CLASS:
			return &classes;
		case SYMBOL_INTERFACE:
			return &interfaces;
		case SYMBOL_ENUM:
			return &enums;
		case SYMBOL_ENUM_CONSTANT:
			return &enumConstants;
		case SYMBOL_FUNCTION:
			return &functions;
		case SYMBOL_FIELD:
			return &fields;
		case SYMBOL_GLOBAL_VARIABLE:
			return &globalVariables;
		case SYMBOL_METHOD:
			return &methods;
		case SYMBOL_NAMESPACE:
			return &namespaces;
		case SYMBOL_STRUCT:
			return &structs;
		case SYMBOL_MACRO:
			return &macros;
		case SYMBOL_TEMPLATE_PARAMETER:
			return &templateParameterTypes;
		case SYMBOL_TYPE_PARAMETER:
			return &typeParameters;
		default:
			break;
		}
		return nullptr;
	}
};

#endif // TEST_PARSER_CLIENT_H
