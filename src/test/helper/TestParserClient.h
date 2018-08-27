#ifndef TEST_PARSER_CLIENT_H
#define TEST_PARSER_CLIENT_H

#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"

class TestParserClient: public ParserClient
{
public:
	Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		AccessKind access, DefinitionKind definitionKind) override
	{
		std::vector<std::wstring>* bin = getBinForSymbolKind(symbolKind);
		if (bin != nullptr)
		{
			bin->push_back(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access));
		}
		return 0;
	}

	Id recordSymbolWithLocation(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionKind definitionKind) override
	{
		std::vector<std::wstring>* bin = getBinForSymbolKind(symbolKind);
		if (bin != nullptr)
		{
			bin->push_back(addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access), location));
		}
		return 0;
	}

	Id recordSymbolWithLocationAndScope(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionKind definitionKind) override
	{
		std::vector<std::wstring>* bin = getBinForSymbolKind(symbolKind);
		if (bin != nullptr)
		{
			bin->push_back(addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access), location, scopeLocation));
		}
		return 0;
	}

	Id recordSymbolWithLocationAndScopeAndSignature(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation, const ParseLocation& signatureLocation,
		AccessKind access, DefinitionKind definitionKind) override
	{
		std::vector<std::wstring>* bin = getBinForSymbolKind(symbolKind);
		if (bin != nullptr)
		{
			bin->push_back(addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access), location, scopeLocation, signatureLocation));
		}
		return 0;
	}

	void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location) override
	{
		std::vector<std::wstring>* referenceContainer = nullptr;
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
		case REFERENCE_ANNOTATION_USAGE:
			referenceContainer = &annotationUses;
			break;
		default:
			break;
		}
		if (referenceContainer != nullptr)
		{
			referenceContainer->push_back(addLocationSuffix(
				contextName.getQualifiedNameWithSignature() + L" -> " + referencedName.getQualifiedNameWithSignature(), location)
			);
		}
	}

	void recordQualifierLocation(
		const NameHierarchy& qualifierName, const ParseLocation& location) override
	{
		qualifiers.push_back(addLocationSuffix(qualifierName.getQualifiedNameWithSignature(), location));
	}

	void recordLocalSymbol(const std::wstring& name, const ParseLocation& location) override
	{
		localSymbols.push_back(addLocationSuffix(name, location));
	}

	void recordFile(const FilePath& filePath, bool indexed) override
	{
		files.insert(filePath.wstr());
	}

	void recordComment(const ParseLocation& location) override
	{
		comments.push_back(addLocationSuffix(L"comment", location));
	}

	std::vector<std::wstring> errors;
	std::vector<std::wstring> qualifiers;

	std::vector<std::wstring> packages;
	std::vector<std::wstring> typedefs;
	std::vector<std::wstring> builtinTypes;
	std::vector<std::wstring> classes;
	std::vector<std::wstring> unions;
	std::vector<std::wstring> interfaces;
	std::vector<std::wstring> annotations;
	std::vector<std::wstring> enums;
	std::vector<std::wstring> enumConstants;
	std::vector<std::wstring> functions;
	std::vector<std::wstring> fields;
	std::vector<std::wstring> globalVariables;
	std::vector<std::wstring> methods;
	std::vector<std::wstring> namespaces;
	std::vector<std::wstring> structs;
	std::vector<std::wstring> macros;
	std::vector<std::wstring> templateParameterTypes;
	std::vector<std::wstring> typeParameters;
	std::vector<std::wstring> localSymbols;
	std::set<std::wstring> files;
	std::vector<std::wstring> comments;

	std::vector<std::wstring> inheritances;
	std::vector<std::wstring> overrides;
	std::vector<std::wstring> calls;
	std::vector<std::wstring> usages;	// for variables
	std::vector<std::wstring> typeUses;	// for types
	std::vector<std::wstring> macroUses;
	std::vector<std::wstring> annotationUses;
	std::vector<std::wstring> templateArgumentTypes;
	std::vector<std::wstring> typeArguments;
	std::vector<std::wstring> templateDefaultArgumentTypes;
	std::vector<std::wstring> templateSpecializations;
	std::vector<std::wstring> templateMemberSpecializations;
	std::vector<std::wstring> includes;
	std::vector<std::wstring> imports;

private:
	void doRecordError(
		const ParseLocation& location,
		const std::wstring& message,
		bool fatal,
		bool indexed,
		const FilePath& translationUnit) override
	{
		if (location.isValid())
		{
			errors.push_back(addLocationSuffix(message, location));
		}
	}

	std::vector<std::wstring>* getBinForSymbolKind(SymbolKind symbolType)
	{
		switch (symbolType)
		{
		case SYMBOL_PACKAGE:
			return &packages;
		case SYMBOL_TYPEDEF:
			return &typedefs;
		case SYMBOL_BUILTIN_TYPE:
			return &builtinTypes;
		case SYMBOL_CLASS:
			return &classes;
		case SYMBOL_UNION:
			return &unions;
		case SYMBOL_INTERFACE:
			return &interfaces;
		case SYMBOL_ANNOTATION:
			return &annotations;
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
