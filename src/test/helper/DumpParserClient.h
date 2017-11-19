#ifndef DUMP_PARSER_CLIENT_H
#define DUMP_PARSER_CLIENT_H

#include "data/parser/ParseLocation.h"
#include "data/parser/ParserClient.h"

#include <boost/filesystem.hpp>

class DumpParserClient : public ParserClient
{
public:
	DumpParserClient()
		: m_lines("")
	{
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		AccessKind access, DefinitionKind definitionKind) override
	{
		recordLine(symbolKindToString(symbolKind) + " " + addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + "\n");
		return 0;
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location,
		AccessKind access, DefinitionKind definitionKind) override
	{
		recordLine(symbolKindToString(symbolKind) + " " + addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + " [" + location.filePath.fileName(), location) + "]\n");
		return 0;
	}

	virtual Id recordSymbol(
		const NameHierarchy& symbolName, SymbolKind symbolKind,
		const ParseLocation& location, const ParseLocation& scopeLocation,
		AccessKind access, DefinitionKind definitionKind) override
	{
		recordLine(symbolKindToString(symbolKind) + " " + addLocationSuffix(addAccessPrefix(symbolName.getQualifiedNameWithSignature(), access) + " [" + location.filePath.fileName(), location, scopeLocation) + "]\n");
		return 0;
	}

	void recordReference(
		ReferenceKind referenceKind, const NameHierarchy& referencedName, const NameHierarchy& contextName,
		const ParseLocation& location) override
	{
		std::string contextNameString = contextName.getQualifiedNameWithSignature();
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
		recordLine(referenceKindToString(referenceKind) + " " + addLocationSuffix(contextNameString + " -> " + referencedName.getQualifiedNameWithSignature() + " [" + location.filePath.fileName(), location) + "]\n");
	}

	void recordQualifierLocation(const NameHierarchy& qualifierName, const ParseLocation& location) override
	{
		recordLine("QUALIFIER: " + addLocationSuffix(qualifierName.getQualifiedNameWithSignature() + " [" + location.filePath.fileName(), location) + "]\n");
	}

	virtual void recordLocalSymbol(const std::string& name, const ParseLocation& location) override
	{
		recordLine("LOCAL_SYMBOL: " + addLocationSuffix(name + " [" + location.filePath.fileName(), location) + "]\n");
	}

	virtual void recordFile(const FileInfo& fileInfo) override
	{
		recordLine("FILE: " + fileInfo.path.fileName() + "\n");
	}

	virtual void recordComment(const ParseLocation& location) override
	{
		recordLine("COMMENT: " + addLocationSuffix("comment [" + location.filePath.fileName(), location) + "]\n");
	}

	std::string m_lines;

private:
	virtual void doRecordError(const ParseLocation& location, const std::string& message, const std::string& commandline,
		bool fatal, bool indexed) override
	{
		recordLine("ERROR: " + addLocationSuffix(message + " [" + location.filePath.fileName(), location) + "]\n");
	}

	void recordLine(const std::string& message)
	{
		if (m_recordedLines.find(message) == m_recordedLines.end())
		{
			m_recordedLines.insert(message);
			m_lines += message;
		}
	}

	std::string symbolKindToString(SymbolKind symbolKind) const
	{
		switch (symbolKind)
		{
		case SYMBOL_BUILTIN_TYPE:
			return "SYMBOL_BUILTIN_TYPE";
		case SYMBOL_CLASS:
			return "SYMBOL_CLASS";
		case SYMBOL_ENUM:
			return "SYMBOL_ENUM";
		case SYMBOL_ENUM_CONSTANT:
			return "SYMBOL_ENUM_CONSTANT";
		case SYMBOL_FIELD:
			return "SYMBOL_FIELD";
		case SYMBOL_FUNCTION:
			return "SYMBOL_FUNCTION";
		case SYMBOL_GLOBAL_VARIABLE:
			return "SYMBOL_GLOBAL_VARIABLE";
		case SYMBOL_INTERFACE:
			return "SYMBOL_INTERFACE";
		case SYMBOL_MACRO:
			return "SYMBOL_MACRO";
		case SYMBOL_METHOD:
			return "SYMBOL_METHOD";
		case SYMBOL_NAMESPACE:
			return "SYMBOL_NAMESPACE";
		case SYMBOL_PACKAGE:
			return "SYMBOL_PACKAGE";
		case SYMBOL_STRUCT:
			return "SYMBOL_STRUCT";
		case SYMBOL_TEMPLATE_PARAMETER:
			return "SYMBOL_TEMPLATE_PARAMETER";
		case SYMBOL_TYPEDEF:
			return "SYMBOL_TYPEDEF";
		case SYMBOL_TYPE_PARAMETER:
			return "SYMBOL_TYPE_PARAMETER";
		case SYMBOL_UNION:
			return "SYMBOL_UNION";
		default:
			break;
		}
		return "SYMBOL_NON_INDEXED";
	}

	std::string referenceKindToString(ReferenceKind referenceKind) const
	{
		switch (referenceKind)
		{
		case REFERENCE_TYPE_USAGE:
			return "REFERENCE_TYPE_USAGE";
		case REFERENCE_USAGE:
			return "REFERENCE_USAGE";
		case REFERENCE_CALL:
			return "REFERENCE_CALL";
		case REFERENCE_INHERITANCE:
			return "REFERENCE_INHERITANCE";
		case REFERENCE_OVERRIDE:
			return "REFERENCE_OVERRIDE";
		case REFERENCE_TEMPLATE_ARGUMENT:
			return "REFERENCE_TEMPLATE_ARGUMENT";
		case REFERENCE_TYPE_ARGUMENT:
			return "REFERENCE_TYPE_ARGUMENT";
		case REFERENCE_TEMPLATE_DEFAULT_ARGUMENT:
			return "REFERENCE_TEMPLATE_DEFAULT_ARGUMENT";
		case REFERENCE_TEMPLATE_SPECIALIZATION:
			return "REFERENCE_TEMPLATE_SPECIALIZATION";
		case REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION:
			return "REFERENCE_TEMPLATE_MEMBER_SPECIALIZATION";
		case REFERENCE_INCLUDE:
			return "REFERENCE_INCLUDE";
		case REFERENCE_IMPORT:
			return "REFERENCE_IMPORT";
		case REFERENCE_MACRO_USAGE:
			return "REFERENCE_MACRO_USAGE";
		default:
			break;
		}
		return "REFERENCE_UNDEFINED";
	}

	std::set<std::string> m_recordedLines;
};

#endif // DUMP_PARSER_CLIENT_H
