#include "CodeblocksProject.h"

#include "tinyxml.h"

#include "ApplicationSettings.h"
#include "CodeblocksCompiler.h"
#include "CodeblocksTarget.h"
#include "CodeblocksUnit.h"
#include "IndexerCommandCxx.h"
#include "OrderedCache.h"
#include "SourceGroupSettingsCxxCodeblocks.h"
#include "TextAccess.h"
#include "logging.h"
#include "utility.h"
#include "utilityString.h"

namespace Codeblocks
{
std::shared_ptr<Project> Project::load(const FilePath& projectFilePath)
{
	return load(TextAccess::createFromFile(projectFilePath));
}

std::shared_ptr<Project> Project::load(std::shared_ptr<TextAccess> xmlAccess)
{
	if (!xmlAccess)
	{
		return std::shared_ptr<Project>();
	}

	std::shared_ptr<Project> project(new Project(xmlAccess->getFilePath()));

	TiXmlDocument doc;
	doc.Parse(xmlAccess->getText().c_str(), 0, TIXML_ENCODING_UTF8);
	if (doc.Error())
	{
		LOG_ERROR(
			"Unable to parse Code::Blocks project because of an error in row " +
			std::to_string(doc.ErrorRow()) + ", col " + std::to_string(doc.ErrorCol()) + ": " +
			std::string(doc.ErrorDesc()));
		return project;
	}

	TiXmlElement* codeBlocksProjectFileElement;
	TiXmlHandle docHandle(&doc);
	{
		codeBlocksProjectFileElement =
			docHandle.FirstChildElement("CodeBlocks_project_file").ToElement();
		if (codeBlocksProjectFileElement == nullptr)
		{
			codeBlocksProjectFileElement =
				docHandle.FirstChildElement("Code::Blocks_project_file").ToElement();
		}
		if (codeBlocksProjectFileElement == nullptr)
		{
			LOG_ERROR("Unable to find root node in Code::Blocks project.");
			return project;
		}
	}

	{
		TiXmlElement* versionElement = codeBlocksProjectFileElement->FirstChildElement(
			"FileVersion");

		if (versionElement->QueryIntAttribute("major", &project->m_versionMajor) != TIXML_SUCCESS)
		{
			LOG_ERROR("Unable to find \"Project\" node in Code::Blocks project.");
			return project;
		}

		if (versionElement->QueryIntAttribute("minor", &project->m_versionMinor) != TIXML_SUCCESS)
		{
			LOG_ERROR("Unable to find \"Project\" node in Code::Blocks project.");
			return project;
		}
	}

	const TiXmlElement* projectElement = codeBlocksProjectFileElement->FirstChildElement("Project");
	if (codeBlocksProjectFileElement == nullptr)
	{
		LOG_ERROR("Unable to find \"Project\" node in Code::Blocks project.");
		return project;
	}

	{
		const TiXmlElement* optionElement = projectElement->FirstChildElement("Option");
		while (optionElement)
		{
			{
				const char* value = optionElement->Attribute("title");
				if (value)
				{
					project->m_title = stringToCompilerVarType(value);
				}
			}

			optionElement = optionElement->NextSiblingElement("Option");
		}
	}

	{
		const TiXmlElement* buildElement = projectElement->FirstChildElement("Build");
		const TiXmlElement* targetElement = buildElement->FirstChildElement(
			Target::getXmlElementName().c_str());
		while (targetElement)
		{
			if (std::shared_ptr<Target> unit = Target::create(targetElement))
			{
				project->m_targets.push_back(unit);
			}

			targetElement = targetElement->NextSiblingElement(Target::getXmlElementName().c_str());
		}
	}

	{
		const TiXmlElement* unitElement = projectElement->FirstChildElement(
			Unit::getXmlElementName().c_str());
		while (unitElement)
		{
			if (std::shared_ptr<Unit> unit = Unit::create(unitElement))
			{
				project->m_units.push_back(unit);
			}

			unitElement = unitElement->NextSiblingElement(Unit::getXmlElementName().c_str());
		}
	}

	return project;
}

std::set<FilePath> Project::getAllSourceFilePathsCanonical(
	const std::vector<std::wstring>& sourceExtensions) const
{
	const std::set<std::wstring> lowerSourceExtensions = utility::toSet(
		utility::convert<std::wstring, std::wstring>(
			sourceExtensions, [](const std::wstring& e) { return utility::toLowerCase(e); }));

	std::set<FilePath> filePaths;
	std::set<FilePath> nonTargetFilePaths;
	for (std::shared_ptr<const Unit> unit: m_units)
	{
		if (unit && unit->getCompile())
		{
			FilePath filePath(unit->getCanonicalFilePath(m_projectFilePath.getParentDirectory()));
			if (lowerSourceExtensions.find(filePath.getLowerCase().extension()) !=
				lowerSourceExtensions.end())
			{
				if (unit->getTargetNames().size())
				{
					filePaths.insert(filePath);
				}
				else
				{
					nonTargetFilePaths.insert(filePath);
				}
			}
		}
	}

	if (!filePaths.size())
	{
		return nonTargetFilePaths;
	}

	return filePaths;
}

std::set<FilePath> Project::getAllCxxHeaderSearchPathsCanonical() const
{
	std::set<std::wstring> usedTargetNames;
	for (std::shared_ptr<const Unit> unit: m_units)
	{
		if (unit && unit->getCompile())
		{
			utility::append(usedTargetNames, unit->getTargetNames());
		}
	}

	OrderedCache<FilePath, FilePath> canonicalDirectoryPathCache(
		[](const FilePath& path) { return path.getCanonical(); });

	std::set<FilePath> paths;
	for (std::shared_ptr<const Target> target: m_targets)
	{
		if (target && usedTargetNames.find(target->getTitle()) != usedTargetNames.end())
		{
			if (std::shared_ptr<const Compiler> compiler = target->getCompiler())
			{
				for (const std::wstring& directory: compiler->getDirectories())
				{
					FilePath path(directory);
					if (path.isAbsolute())
					{
						paths.insert(canonicalDirectoryPathCache.getValue(path));
					}
				}
			}
		}
	}
	return paths;
}

std::vector<std::shared_ptr<IndexerCommandCxx>> Project::getIndexerCommands(
	std::shared_ptr<const SourceGroupSettingsCxxCodeblocks> sourceGroupSettings,
	std::shared_ptr<const ApplicationSettings> appSettings) const
{
	const std::set<std::wstring> lowerSourceExtensions = utility::toSet(
		utility::convert<std::wstring, std::wstring>(
			sourceGroupSettings->getSourceExtensions(),
			[](const std::wstring& e) { return utility::toLowerCase(e); }));

	const std::set<FilePath> indexedHeaderPaths = utility::toSet(
		sourceGroupSettings->getIndexedHeaderPathsExpandedAndAbsolute());

	const std::set<FilePathFilter> excludeFilters = utility::toSet(
		sourceGroupSettings->getExcludeFiltersExpandedAndAbsolute());

	const std::vector<FilePath> systemHeaderSearchPaths = utility::concat(
		sourceGroupSettings->getHeaderSearchPathsExpandedAndAbsolute(),
		appSettings->getHeaderSearchPathsExpanded());

	const std::vector<FilePath> frameworkSearchPaths = utility::concat(
		sourceGroupSettings->getFrameworkSearchPathsExpandedAndAbsolute(),
		appSettings->getFrameworkSearchPathsExpanded());

	OrderedCache<std::wstring, std::vector<std::wstring>> optionsCache(
		[&](const std::wstring& targetName) {
			std::vector<std::wstring> compilerFlags;
			for (std::shared_ptr<Target> target: m_targets)
			{
				if (target->getTitle() == targetName)
				{
					compilerFlags = utility::concat(
						IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(
							utility::convert<std::wstring, FilePath>(
								target->getCompiler()->getDirectories())),
						target->getCompiler()->getOptions());
					break;
				}
			}

			utility::append(
				compilerFlags,
				IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(
					systemHeaderSearchPaths));
			utility::append(
				compilerFlags,
				IndexerCommandCxx::getCompilerFlagsForFrameworkSearchPaths(frameworkSearchPaths));
			utility::append(compilerFlags, sourceGroupSettings->getCompilerFlags());
			return compilerFlags;
		});

	std::vector<std::shared_ptr<IndexerCommandCxx>> indexerCommands;
	std::vector<std::shared_ptr<IndexerCommandCxx>> nonTargetIndexerCommands;
	for (std::shared_ptr<Unit> unit: m_units)
	{
		if (!unit || !unit->getCompile())
		{
			continue;
		}

		const FilePath filePath = unit->getCanonicalFilePath(m_projectFilePath.getParentDirectory());
		if (lowerSourceExtensions.find(filePath.getLowerCase().extension()) ==
			lowerSourceExtensions.end())
		{
			continue;
		}

		std::wstring languageStandard;
		switch (unit->getCompilerVar())
		{
		case COMPILER_VAR_C:
			languageStandard = sourceGroupSettings->getCStandard();
			break;
		case COMPILER_VAR_CPP:
			languageStandard = sourceGroupSettings->getCppStandard();
			break;
		default:
			continue;
		}

		if (!unit->getTargetNames().size())
		{
			nonTargetIndexerCommands.push_back(std::make_shared<IndexerCommandCxx>(
				filePath,
				utility::concat(indexedHeaderPaths, {filePath}),
				excludeFilters,
				std::set<FilePathFilter>(),
				sourceGroupSettings->getCodeblocksProjectPathExpandedAndAbsolute().getParentDirectory(),
				utility::concat(
					optionsCache.getValue(L""),
					std::vector<std::wstring>(
						{IndexerCommandCxx::getCompilerFlagLanguageStandard(languageStandard),
						 filePath.wstr()}))));
			continue;
		}

		for (const std::wstring& targetName: unit->getTargetNames())
		{
			indexerCommands.push_back(std::make_shared<IndexerCommandCxx>(
				filePath,
				utility::concat(indexedHeaderPaths, {filePath}),
				excludeFilters,
				std::set<FilePathFilter>(),
				sourceGroupSettings->getCodeblocksProjectPathExpandedAndAbsolute().getParentDirectory(),
				utility::concat(
					optionsCache.getValue(targetName),
					std::vector<std::wstring>(
						{IndexerCommandCxx::getCompilerFlagLanguageStandard(languageStandard),
						 filePath.wstr()}))));
		}
	}

	if (!indexerCommands.size())
	{
		return nonTargetIndexerCommands;
	}

	return indexerCommands;
}

Project::Project(const FilePath& projectFilePath): m_projectFilePath(projectFilePath) {}
}	 // namespace Codeblocks
