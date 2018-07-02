#ifndef DUMP_PARSER_CLIENT_H
#define DUMP_PARSER_CLIENT_H

#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"

#include <boost/filesystem.hpp>

class DumpParserClient : public ParserClient
{
public:
	DumpParserClient()
		: m_lines(L"")
	{
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		AccessKind access, DefinitionKind definitionKind) override
	{
		recordLine(symbolKindToString(symbolKind) + L" " + addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + L"\n");
		return 0;
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionKind definitionKind) override
	{
		recordLine(symbolKindToString(symbolKind) + L" " + addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + L" [" + location.filePath.fileName(), location) + L"]\n");
		return 0;
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionKind definitionKind) override
	{
		recordLine(symbolKindToString(symbolKind) + L" " + addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + L" [" + location.filePath.fileName(), location, scopeLocation) + L"]\n");
		return 0;
	}

	void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location) override
	{
		std::wstring referencedNameString = referencedName.getQualifiedNameWithSignature();
		try
		{
			if (FilePath(referencedNameString).exists())
			{
				referencedNameString = FilePath(referencedNameString).fileName();
			}
		}
		catch (const boost::filesystem::filesystem_error& e)
		{
			// do nothing and use the old contectNameString
		}

		std::wstring contextNameString = contextName.getQualifiedNameWithSignature();
		try
		{
			if (FilePath(contextNameString).exists())
			{
				contextNameString = FilePath(contextNameString).fileName();
			}
		}
		catch (const boost::filesystem::filesystem_error& e)
		{
			// do nothing and use the old contectNameString
		}

		recordLine(referenceKindToString(referenceKind) + L" " + addLocationSuffix(contextNameString + L" -> " + referencedNameString + L" [" + location.filePath.fileName(), location) + L"]\n");
	}

	void recordQualifierLocation(const NameHierarchy& qualifierName, const ParseLocation& location) override
	{
		recordLine(L"QUALIFIER: " + addLocationSuffix(qualifierName.getQualifiedNameWithSignature() + L" [" + location.filePath.fileName(), location) + L"]\n");
	}

	virtual void recordLocalSymbol(const std::wstring& name, const ParseLocation& location) override
	{
		recordLine(L"LOCAL_SYMBOL: " + addLocationSuffix(name + L" [" + location.filePath.fileName(), location) + L"]\n");
	}

	virtual void recordFile(const FileInfo& fileInfo, bool indexed) override
	{
		recordLine(L"FILE: " + fileInfo.path.fileName() + (indexed ? L"" : L" non-indexed") + L"\n");
	}

	virtual void recordComment(const ParseLocation& location) override
	{
		recordLine(L"COMMENT: " + addLocationSuffix(L"comment [" + location.filePath.fileName(), location) + L"]\n");
	}

	std::wstring m_lines;

private:
	virtual void doRecordError(const ParseLocation& location, const std::wstring& message,
		bool fatal, bool indexed, const FilePath& translationUnit) override
	{
		recordLine(L"ERROR: " + addLocationSuffix(message + L" [" + location.filePath.fileName(), location) + L"]\n");
	}

	void recordLine(const std::wstring& message)
	{
		if (m_recordedLines.find(message) == m_recordedLines.end())
		{
			m_recordedLines.insert(message);
			m_lines += message;
		}
	}

	std::wstring symbolKindToString(SymbolKind symbolKind) const
	{
		switch (symbolKind)
		{
		case SYMBOL_BUILTIN_TYPE:
			return L"SYMBOL_BUILTIN_TYPE";
		case SYMBOL_CLASS:
			return L"SYMBOL_CLASS";
		case SYMBOL_ENUM:
			return L"SYMBOL_ENUM";
		case SYMBOL_ENUM_CONSTANT:
			return L"SYMBOL_ENUM_CONSTANT";
		case SYMBOL_FIELD:
			return L"SYMBOL_FIELD";
		case SYMBOL_FUNCTION:
			return L"SYMBOL_FUNCTION";
		case SYMBOL_GLOBAL_VARIABLE:
			return L"SYMBOL_GLOBAL_VARIABLE";
		case SYMBOL_INTERFACE:
			return L"SYMBOL_INTERFACE";
		case SYMBOL_MACRO:
			return L"SYMBOL_MACRO";
		case SYMBOL_METHOD:
			return L"SYMBOL_METHOD";
		case SYMBOL_NAMESPACE:
			return L"SYMBOL_NAMESPACE";
		case SYMBOL_PACKAGE:
			return L"SYMBOL_PACKAGE";
		case SYMBOL_STRUCT:
			return L"SYMBOL_STRUCT";
		case SYMBOL_TEMPLATE_PARAMETER:
			return L"SYMBOL_TEMPLATE_PARAMETER";
		case SYMBOL_TYPEDEF:
			return L"SYMBOL_TYPEDEF";
		case SYMBOL_TYPE_PARAMETER:
			return L"SYMBOL_TYPE_PARAMETER";
		case SYMBOL_UNION:
			return L"SYMBOL_UNION";
		default:
			break;
		}
		return L"SYMBOL_NON_INDEXED";
	}

	std::wstring referenceKindToString(ReferenceKind referenceKind) const
	{
		switch (referenceKind)
		{
		case REFERENCE_TYPE_USAGE:
			return L"REFERENCE_TYPE_USAGE";
		case REFERENCE_USAGE:
			return L"REFERENCE_USAGE";
		case REFERENCE_CALL:
			return L"REFERENCE_CALL";
		case REFERENCE_INHERITANCE:
			return L"REFERENCE_INHERITANCE";
		case REFERENCE_OVERRIDE:
			return L"REFERENCE_OVERRIDE";
		case REFERENCE_TEMPLATE_ARGUMENT:
			return L"REFERENCE_TEMPLATE_ARGUMENT";
		case REFERENCE_TYPE_ARGUMENT:
			return L"REFERENCE_TYPE_ARGUMENT";
		case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
			return L"REFERENCE_TEMPLATE_DEFAULT_ARGUMENT";
		case REFERENCE_TEMPLATE_SPECIALIZATION:
			return L"REFERENCE_TEMPLATE_SPECIALIZATION";
		case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION:
			return L"REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION";
		case REFERENCE_INCLUDE:
			return L"REFERENCE_INCLUDE";
		case REFERENCE_IMPORT:
			return L"REFERENCE_IMPORT";
		case REFERENCE_MACRO_USAGE:
			return L"REFERENCE_MACRO_USAGE";
		default:
			break;
		}
		return L"REFERENCE_UNDEFINED";
	}

	std::set<std::wstring> m_recordedLines;
};

#endif // DUMP_PARSER_CLIENT_H
