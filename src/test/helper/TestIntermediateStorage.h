#ifndef TEST_INTERMEDIATE_STORAGE_H
#define TEST_INTERMEDIATE_STORAGE_H

#include <boost/filesystem.hpp>

#include "AccessKind.h"
#include "Edge.h"
#include "FilePath.h"
#include "IntermediateStorage.h"
#include "LocationType.h"
#include "NameHierarchy.h"
#include "NodeType.h"
#include "utilityString.h"

class TestIntermediateStorage: public IntermediateStorage
{
public:
	void generateStringLists()
	{
		std::map<Id, FilePath> filePathMap;
		for (const StorageFile& file: getStorageFiles())
		{
			filePathMap.emplace(file.id, FilePath(file.filePath));
			files.emplace(file.filePath);

			addLine(
				L"FILE: " + FilePath(file.filePath).fileName() +
				(file.indexed ? L"" : L" non-indexed"));
		}

		std::map<Id, StorageComponentAccess> accessMap;
		for (const StorageComponentAccess& access: getComponentAccesses())
		{
			accessMap.emplace(access.nodeId, access);
		}

		std::multimap<Id, Id> occurrenceMap;
		for (const StorageOccurrence& occurence: getStorageOccurrences())
		{
			occurrenceMap.emplace(occurence.sourceLocationId, occurence.elementId);
		}

		std::multimap<Id, StorageSourceLocation> tokenLocationMap;
		std::multimap<Id, StorageSourceLocation> scopeLocationMap;
		std::multimap<Id, StorageSourceLocation> signatureLocationMap;
		std::multimap<Id, StorageSourceLocation> localSymbolLocationMap;
		std::multimap<Id, StorageSourceLocation> qualifierLocationMap;
		std::multimap<Id, StorageSourceLocation> errorLocationMap;
		std::vector<StorageSourceLocation> commentLocations;
		for (const StorageSourceLocation& location: getStorageSourceLocations())
		{
			std::vector<Id> elementIds;
			for (auto it = occurrenceMap.find(location.id);
				 it != occurrenceMap.end() && it->first == location.id;
				 it++)
			{
				elementIds.emplace_back(it->second);
			}

			if (elementIds.empty())
			{
				elementIds.emplace_back(0);
			}

			for (Id elementId: elementIds)
			{
				switch (intToLocationType(location.type))
				{
				case LOCATION_TOKEN:
					if (elementId)
					{
						tokenLocationMap.emplace(elementId, location);
					}
					break;
				case LOCATION_SCOPE:
					if (elementId)
					{
						scopeLocationMap.emplace(elementId, location);
					}
					break;
				case LOCATION_QUALIFIER:
					if (elementId)
					{
						qualifierLocationMap.emplace(elementId, location);
					}
					break;
				case LOCATION_LOCAL_SYMBOL:
					if (elementId)
					{
						localSymbolLocationMap.emplace(elementId, location);
					}
					break;
				case LOCATION_SIGNATURE:
					if (elementId)
					{
						signatureLocationMap.emplace(elementId, location);
					}
					break;
				case LOCATION_ERROR:
					if (elementId)
					{
						errorLocationMap.emplace(elementId, location);
					}
					break;
				case LOCATION_COMMENT:
					commentLocations.emplace_back(location);
					break;
				default:
					break;
				}
			}
		}

		std::map<Id, StorageNode> nodesMap;
		std::set<Id> fileIdMap;
		for (const StorageNode& node: getStorageNodes())
		{
			nodesMap.emplace(node.id, node);

			if (intToNodeKind(node.type) == NODE_FILE)
			{
				fileIdMap.insert(node.id);
			}

			std::wstring nameStr =
				NameHierarchy::deserialize(node.serializedName).getQualifiedNameWithSignature();

			for (auto qualifierLocationIt = qualifierLocationMap.find(node.id);
				 qualifierLocationIt != qualifierLocationMap.end() &&
				 qualifierLocationIt->first == node.id;
				 qualifierLocationIt++)
			{
				std::wstring locStr = addLocationStr(L"", qualifierLocationIt->second);
				qualifiers.emplace_back(nameStr + locStr);
				addLine(
					L"QUALIFIER: " + nameStr +
					addFileName(locStr, filePathMap[qualifierLocationIt->second.fileNodeId]));
			}

			std::vector<std::wstring>* bin = getBinForNodeType(node.type);
			if (bin != nullptr)
			{
				auto accessIt = accessMap.find(node.id);
				if (accessIt != accessMap.end())
				{
					if (intToAccessKind(accessIt->second.type) != ACCESS_TEMPLATE_PARAMETER &&
						intToAccessKind(accessIt->second.type) != ACCESS_TYPE_PARAMETER)
					{
						nameStr = accessKindToString(intToAccessKind(accessIt->second.type)) +
							L' ' + nameStr;
					}
				}

				bool added = false;
				for (auto tokenLocationIt = tokenLocationMap.find(node.id);
					 tokenLocationIt != tokenLocationMap.end() && tokenLocationIt->first == node.id;
					 tokenLocationIt++)
				{
					added = false;
					std::wstring locationStr = addLocationStr(L"", tokenLocationIt->second);

					auto signatureLocationIt = signatureLocationMap.find(node.id);
					if (signatureLocationIt != signatureLocationMap.end() &&
						containsLocation(signatureLocationIt->second, tokenLocationIt->second))
					{
						locationStr = addLocationStr(locationStr, signatureLocationIt->second);
					}

					for (auto scopeLocationIt = scopeLocationMap.find(node.id);
						 scopeLocationIt != scopeLocationMap.end() &&
						 scopeLocationIt->first == node.id;
						 scopeLocationIt++)
					{
						if (containsLocation(scopeLocationIt->second, tokenLocationIt->second))
						{
							std::wstring locStr = addLocationStr(
								locationStr, scopeLocationIt->second);
							bin->emplace_back(nameStr + locStr);
							addLine(
								nodeTypeToString(node.type) + L": " + nameStr +
								addFileName(locStr, filePathMap[tokenLocationIt->second.fileNodeId]));
							added = true;
						}
					}

					if (!added)
					{
						bin->emplace_back(nameStr + locationStr);
						addLine(
							nodeTypeToString(node.type) + L": " + nameStr +
							addFileName(locationStr, filePathMap[tokenLocationIt->second.fileNodeId]));
						added = true;
					}
				}

				if (!added)
				{
					bin->emplace_back(nameStr);
					addLine(nodeTypeToString(node.type) + L": " + nameStr);
				}
			}
		}

		for (const StorageEdge& edge: getStorageEdges())
		{
			auto targetIt = nodesMap.find(edge.targetNodeId);
			if (targetIt == nodesMap.end())
			{
				continue;
			}

			auto sourceIt = nodesMap.find(edge.sourceNodeId);
			if (sourceIt == nodesMap.end())
			{
				continue;
			}

			const StorageNode& target = targetIt->second;
			const StorageNode& source = sourceIt->second;

			std::vector<std::wstring>* bin = getBinForEdgeType(edge.type);
			if (bin != nullptr)
			{
				std::wstring sourceName =
					NameHierarchy::deserialize(source.serializedName).getQualifiedNameWithSignature();
				if (fileIdMap.find(edge.sourceNodeId) != fileIdMap.end() && FilePath(sourceName).exists())
				{
					sourceName = FilePath(sourceName).fileName();
				}

				std::wstring targetName =
					NameHierarchy::deserialize(target.serializedName).getQualifiedNameWithSignature();
				if (fileIdMap.find(edge.targetNodeId) != fileIdMap.end() && FilePath(targetName).exists())
				{
					targetName = FilePath(targetName).fileName();
				}

				std::wstring nameStr = sourceName + L" -> " + targetName;

				bool added = false;
				for (auto tokenLocationIt = tokenLocationMap.find(edge.id);
					 tokenLocationIt != tokenLocationMap.end() && tokenLocationIt->first == edge.id;
					 tokenLocationIt++)
				{
					std::wstring locStr = addLocationStr(L"", tokenLocationIt->second);
					bin->emplace_back(nameStr + locStr);
					addLine(
						edgeTypeToString(edge.type) + L": " + nameStr +
						addFileName(locStr, filePathMap[tokenLocationIt->second.fileNodeId]));
					added = true;
				}

				if (!added)
				{
					bin->emplace_back(nameStr);
					addLine(edgeTypeToString(edge.type) + L": " + nameStr);
				}
			}
		}

		for (const StorageLocalSymbol& localSymbol: getStorageLocalSymbols())
		{
			bool added = false;
			for (auto localSymbolLocationIt = localSymbolLocationMap.find(localSymbol.id);
				 localSymbolLocationIt != localSymbolLocationMap.end() &&
				 localSymbolLocationIt->first == localSymbol.id;
				 localSymbolLocationIt++)
			{
				std::wstring locStr = addLocationStr(L"", localSymbolLocationIt->second);
				localSymbols.emplace_back(localSymbol.name + locStr);
				addLine(
					L"LOCAL_SYMBOL: " + localSymbol.name +
					addFileName(locStr, filePathMap[localSymbolLocationIt->second.fileNodeId]));
				added = true;
			}

			if (!added)
			{
				localSymbols.emplace_back(localSymbol.name);
				addLine(L"LOCAL_SYMBOL: " + localSymbol.name);
			}
		}

		for (const StorageSourceLocation& location: commentLocations)
		{
			std::wstring locStr = addLocationStr(L"", location);
			comments.emplace_back(L"comment" + locStr);
			addLine(L"COMMENT: comment" + addFileName(locStr, filePathMap[location.fileNodeId]));
		}

		for (const StorageError& error: getErrors())
		{
			for (auto errorLocationIt = errorLocationMap.find(error.id);
				 errorLocationIt != errorLocationMap.end() && errorLocationIt->first == error.id;
				 errorLocationIt++)
			{
				std::wstring locStr = addLocationStr(L"", errorLocationIt->second);
				errors.emplace_back(error.message + locStr);
				addLine(
					L"ERROR: " + error.message +
					addFileName(locStr, filePathMap[errorLocationIt->second.fileNodeId]));
			}
		}
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
	std::vector<std::wstring> modules;
	std::vector<std::wstring> structs;
	std::vector<std::wstring> macros;
	std::vector<std::wstring> typeParameters;
	std::vector<std::wstring> localSymbols;
	std::set<std::wstring> files;
	std::vector<std::wstring> comments;

	std::vector<std::wstring> inheritances;
	std::vector<std::wstring> overrides;
	std::vector<std::wstring> calls;
	std::vector<std::wstring> usages;	   // for variables
	std::vector<std::wstring> typeUses;	   // for types
	std::vector<std::wstring> macroUses;
	std::vector<std::wstring> annotationUses;
	std::vector<std::wstring> typeArguments;
	std::vector<std::wstring> templateSpecializations;
	std::vector<std::wstring> includes;
	std::vector<std::wstring> imports;

	std::vector<std::string> m_lines;

private:
	std::wstring nodeTypeToString(int nodeType) const
	{
		switch (intToNodeKind(nodeType))
		{
		case NODE_BUILTIN_TYPE:
			return L"SYMBOL_BUILTIN_TYPE";
		case NODE_CLASS:
			return L"SYMBOL_CLASS";
		case NODE_ENUM:
			return L"SYMBOL_ENUM";
		case NODE_ENUM_CONSTANT:
			return L"SYMBOL_ENUM_CONSTANT";
		case NODE_FIELD:
			return L"SYMBOL_FIELD";
		case NODE_FUNCTION:
			return L"SYMBOL_FUNCTION";
		case NODE_GLOBAL_VARIABLE:
			return L"SYMBOL_GLOBAL_VARIABLE";
		case NODE_INTERFACE:
			return L"SYMBOL_INTERFACE";
		case NODE_MACRO:
			return L"SYMBOL_MACRO";
		case NODE_METHOD:
			return L"SYMBOL_METHOD";
		case NODE_MODULE:
			return L"SYMBOL_MODULE";
		case NODE_NAMESPACE:
			return L"SYMBOL_NAMESPACE";
		case NODE_PACKAGE:
			return L"SYMBOL_PACKAGE";
		case NODE_STRUCT:
			return L"SYMBOL_STRUCT";
		case NODE_TYPEDEF:
			return L"SYMBOL_TYPEDEF";
		case NODE_TYPE_PARAMETER:
			return L"SYMBOL_TYPE_PARAMETER";
		case NODE_UNION:
			return L"SYMBOL_UNION";
		default:
			break;
		}
		return L"SYMBOL_NON_INDEXED";
	}

	std::wstring edgeTypeToString(int edgeType) const
	{
		switch (Edge::intToType(edgeType))
		{
		case Edge::EDGE_TYPE_USAGE:
			return L"REFERENCE_TYPE_USAGE";
		case Edge::EDGE_USAGE:
			return L"REFERENCE_USAGE";
		case Edge::EDGE_CALL:
			return L"REFERENCE_CALL";
		case Edge::EDGE_INHERITANCE:
			return L"REFERENCE_INHERITANCE";
		case Edge::EDGE_OVERRIDE:
			return L"REFERENCE_OVERRIDE";
		case Edge::EDGE_TYPE_ARGUMENT:
			return L"REFERENCE_TYPE_ARGUMENT";
		case Edge::EDGE_TEMPLATE_SPECIALIZATION:
			return L"REFERENCE_TEMPLATE_SPECIALIZATION";
		case Edge::EDGE_INCLUDE:
			return L"REFERENCE_INCLUDE";
		case Edge::EDGE_IMPORT:
			return L"REFERENCE_IMPORT";
		case Edge::EDGE_MACRO_USAGE:
			return L"REFERENCE_MACRO_USAGE";
		case Edge::EDGE_ANNOTATION_USAGE:
			return L"REFERENCE_ANNOTATION_USAGE";
		default:
			break;
		}
		return L"REFERENCE_UNDEFINED";
	}

	std::vector<std::wstring>* getBinForNodeType(int nodeType)
	{
		switch (intToNodeKind(nodeType))
		{
		case NODE_PACKAGE:
			return &packages;
		case NODE_TYPEDEF:
			return &typedefs;
		case NODE_BUILTIN_TYPE:
			return &builtinTypes;
		case NODE_CLASS:
			return &classes;
		case NODE_UNION:
			return &unions;
		case NODE_INTERFACE:
			return &interfaces;
		case NODE_ANNOTATION:
			return &annotations;
		case NODE_ENUM:
			return &enums;
		case NODE_ENUM_CONSTANT:
			return &enumConstants;
		case NODE_FUNCTION:
			return &functions;
		case NODE_FIELD:
			return &fields;
		case NODE_GLOBAL_VARIABLE:
			return &globalVariables;
		case NODE_METHOD:
			return &methods;
		case NODE_MODULE:
			return &modules;
		case NODE_NAMESPACE:
			return &namespaces;
		case NODE_STRUCT:
			return &structs;
		case NODE_MACRO:
			return &macros;
		case NODE_TYPE_PARAMETER:
			return &typeParameters;
		default:
			break;
		}
		return nullptr;
	}

	std::vector<std::wstring>* getBinForEdgeType(int edgeType)
	{
		switch (Edge::intToType(edgeType))
		{
		case Edge::EDGE_TYPE_USAGE:
			return &typeUses;
		case Edge::EDGE_USAGE:
			return &usages;
		case Edge::EDGE_CALL:
			return &calls;
		case Edge::EDGE_INHERITANCE:
			return &inheritances;
		case Edge::EDGE_OVERRIDE:
			return &overrides;
		case Edge::EDGE_TYPE_ARGUMENT:
			return &typeArguments;
		case Edge::EDGE_TEMPLATE_SPECIALIZATION:
			return &templateSpecializations;
		case Edge::EDGE_INCLUDE:
			return &includes;
		case Edge::EDGE_IMPORT:
			return &imports;
		case Edge::EDGE_MACRO_USAGE:
			return &macroUses;
		case Edge::EDGE_ANNOTATION_USAGE:
			return &annotationUses;
		default:
			break;
		}
		return nullptr;
	}

	std::wstring addLocationStr(const std::wstring& locationStr, const StorageSourceLocation& loc) const
	{
		return L" <" + std::to_wstring(loc.startLine) + L':' + std::to_wstring(loc.startCol) +
			locationStr + L' ' + std::to_wstring(loc.endLine) + L':' + std::to_wstring(loc.endCol) +
			L'>';
	}

	std::wstring addFileName(const std::wstring& locationStr, const FilePath& filePath) const
	{
		return L" [" + filePath.fileName() + locationStr + L']';
	}

	bool containsLocation(const StorageSourceLocation& out, const StorageSourceLocation& in) const
	{
		if (out.startLine > in.startLine)
		{
			return false;
		}

		if (out.startLine == in.startLine && out.startCol > in.startCol)
		{
			return false;
		}

		if (out.endLine < in.endLine)
		{
			return false;
		}

		if (out.endLine == in.endLine && out.endCol < in.endCol)
		{
			return false;
		}

		return true;
	}

	void addLine(const std::wstring& message)
	{
		m_lines.emplace_back(utility::encodeToUtf8(message) + '\n');
	}
};

#endif	  // TEST_INTERMEDIATE_STORAGE_H
