#include "SourceGroupCxxEmpty.h"

#include "ApplicationSettings.h"
#include "CxxCompilationDatabaseSingle.h"
#include "CxxIndexerCommandProvider.h"
#include "CxxParser.h"
#include "DialogView.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "GeneratePCHAction.h"
#include "IndexerCommandCxx.h"
#include "SingleFrontendActionFactory.h"
#include "SourceGroupSettingsCEmpty.h"
#include "SourceGroupSettingsCppEmpty.h"
#include "SourceGroupSettingsWithCppStandard.h"
#include "SourceGroupSettingsWithCStandard.h"
#include "TaskLambda.h"
#include "utility.h"

SourceGroupCxxEmpty::SourceGroupCxxEmpty(std::shared_ptr<SourceGroupSettingsCxx> settings)
	: m_settings(settings)
{
}

std::set<FilePath> SourceGroupCxxEmpty::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	std::vector<FilePath> indexedPaths;
	std::vector<FilePathFilter> excludeFilters;
	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		indexedPaths = settings->getSourcePathsExpandedAndAbsolute();
		excludeFilters = settings->getExcludeFiltersExpandedAndAbsolute();
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		indexedPaths = settings->getSourcePathsExpandedAndAbsolute();
		excludeFilters = settings->getExcludeFiltersExpandedAndAbsolute();
	}

	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		std::set<FilePath>(),
		utility::toSet(indexedPaths),
		excludeFilters
	);
}

std::set<FilePath> SourceGroupCxxEmpty::getAllSourceFilePaths() const
{
	FileManager fileManager;
	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		fileManager.update(
			settings->getSourcePathsExpandedAndAbsolute(),
			settings->getExcludeFiltersExpandedAndAbsolute(),
			settings->getSourceExtensions()
		);
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		fileManager.update(
			settings->getSourcePathsExpandedAndAbsolute(),
			settings->getExcludeFiltersExpandedAndAbsolute(),
			settings->getSourceExtensions()
		);
	}

	return fileManager.getAllSourceFilePaths();
}

std::shared_ptr<IndexerCommandProvider> SourceGroupCxxEmpty::getIndexerCommandProvider(const std::set<FilePath>& filesToIndex) const
{
	std::set<FilePath> indexedPaths;
	std::set<FilePathFilter> excludeFilters;
	FilePath pchInputFilePath;
	FilePath pchDependenciesDirectoryPath;
	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		indexedPaths = utility::toSet(settings->getSourcePathsExpandedAndAbsolute());
		excludeFilters = utility::toSet(settings->getExcludeFiltersExpandedAndAbsolute());
		pchInputFilePath = settings->getPchInputFilePathExpandedAndAbsolute();
		pchDependenciesDirectoryPath = settings->getPchDependenciesDirectoryPath();
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		indexedPaths = utility::toSet(settings->getSourcePathsExpandedAndAbsolute());
		excludeFilters = utility::toSet(settings->getExcludeFiltersExpandedAndAbsolute());
		pchInputFilePath = settings->getPchInputFilePathExpandedAndAbsolute();
		pchDependenciesDirectoryPath = settings->getPchDependenciesDirectoryPath();
	}

	std::vector<std::wstring> compilerFlags = getCompilerFlags();

	if (!pchInputFilePath.empty() && !pchDependenciesDirectoryPath.empty())
	{
		const FilePath pchOutputFilePath = pchDependenciesDirectoryPath.getConcatenated(pchInputFilePath.fileName()).replaceExtension(L"pch");
		compilerFlags.push_back(L"-fallow-pch-with-compiler-errors");
		compilerFlags.push_back(L"-include-pch");
		compilerFlags.push_back(pchOutputFilePath.wstr());
	}

	std::shared_ptr<CxxIndexerCommandProvider> provider = std::make_shared<CxxIndexerCommandProvider>();
	for (const FilePath& sourcePath: getAllSourceFilePaths())
	{
		if (filesToIndex.find(sourcePath) != filesToIndex.end())
		{
			provider->addCommand(std::make_shared<IndexerCommandCxx>(
				sourcePath,
				indexedPaths,
				excludeFilters,
				std::set<FilePathFilter>(),
				m_settings->getProjectDirectoryPath(),
				utility::concat(compilerFlags, sourcePath.wstr())
			));
		}
	}

	return provider;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxEmpty::getIndexerCommands(const std::set<FilePath>& filesToIndex) const
{
	return getIndexerCommandProvider(filesToIndex)->consumeAllCommands();
}

std::shared_ptr<Task> SourceGroupCxxEmpty::getPreIndexTask(std::shared_ptr<DialogView> dialogView) const
{
	FilePath pchInputFilePath;
	FilePath pchDependenciesDirectoryPath;
	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		pchInputFilePath = settings->getPchInputFilePathExpandedAndAbsolute();
		pchDependenciesDirectoryPath = settings->getPchDependenciesDirectoryPath();
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		pchInputFilePath = settings->getPchInputFilePathExpandedAndAbsolute();
		pchDependenciesDirectoryPath = settings->getPchDependenciesDirectoryPath();
	}

	if (pchInputFilePath.empty() || pchDependenciesDirectoryPath.empty())
	{
		return std::make_shared<TaskLambda>([]() {});
	}

	if (!pchInputFilePath.exists())
	{
		LOG_ERROR(L"Precompiled header input file \"" + pchInputFilePath.wstr() + L"\" does not exist.");
		return std::make_shared<TaskLambda>([]() {});
	}

	const FilePath pchOutputFilePath = pchDependenciesDirectoryPath.getConcatenated(pchInputFilePath.fileName()).replaceExtension(L"pch");

	std::vector<std::wstring> compilerFlags = getCompilerFlags();
	compilerFlags.push_back(pchInputFilePath.wstr());
	compilerFlags.push_back(L"-emit-pch");
	compilerFlags.push_back(L"-o");
	compilerFlags.push_back(pchOutputFilePath.wstr());

	return std::make_shared<TaskLambda>(
		[dialogView, pchInputFilePath, pchOutputFilePath, compilerFlags]()
		{
			dialogView->showUnknownProgressDialog(L"Preparing Indexing", L"Processing Precompiled Headers");
			LOG_INFO(
				L"Generating precompiled header output for input file \"" + pchInputFilePath.wstr() +
				L"\" at location \"" + pchOutputFilePath.wstr() + L"\""
			);

			CxxParser::initializeLLVM();

			if (!pchOutputFilePath.getParentDirectory().exists())
			{
				FileSystem::createDirectory(pchOutputFilePath.getParentDirectory());
			}

			clang::tooling::CompileCommand pchCommand;
			pchCommand.Filename = utility::encodeToUtf8(pchInputFilePath.fileName());
			pchCommand.Directory = pchOutputFilePath.getParentDirectory().str();
			// DON'T use "-fsyntax-only" here because it will cause the output file to be erased
			pchCommand.CommandLine = utility::concat({ "clang-tool" },  CxxParser::getCommandlineArgumentsEssential(compilerFlags));

			CxxCompilationDatabaseSingle compilationDatabase(pchCommand);
			clang::tooling::ClangTool tool(compilationDatabase, std::vector<std::string>(1, utility::encodeToUtf8(pchInputFilePath.wstr())));
			GeneratePCHAction* action = new GeneratePCHAction();
			tool.clearArgumentsAdjusters();
			tool.run(new SingleFrontendActionFactory(action));
		}
	);
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxEmpty::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxEmpty::getSourceGroupSettings() const
{
	return m_settings;
}

std::vector<std::wstring> SourceGroupCxxEmpty::getCompilerFlags() const
{
	std::vector<std::wstring> compilerFlags;

	std::shared_ptr<ApplicationSettings> appSettings = ApplicationSettings::getInstance();
	std::set<FilePath> indexedPaths;
	std::wstring targetFlag;
	std::wstring languageStandard = SourceGroupSettingsWithCppStandard::getDefaultCppStandardStatic();

	if (std::shared_ptr<SourceGroupSettingsCEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCEmpty>(m_settings))
	{
		indexedPaths = utility::toSet(settings->getSourcePathsExpandedAndAbsolute());
		targetFlag = settings->getTargetFlag();
		languageStandard = settings->getCStandard();
	}
	else if (std::shared_ptr<SourceGroupSettingsCppEmpty> settings =
		std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		indexedPaths = utility::toSet(settings->getSourcePathsExpandedAndAbsolute());
		targetFlag = settings->getTargetFlag();
		languageStandard = settings->getCppStandard();
	}
	else
	{
		LOG_ERROR(L"Source group doesn't specify language standard. Falling back to \"" + languageStandard + L"\".");
	}

	if (!targetFlag.empty())
	{
		compilerFlags.push_back(targetFlag);
	}

	compilerFlags.push_back(IndexerCommandCxx::getCompilerFlagLanguageStandard(languageStandard));

	if (std::dynamic_pointer_cast<SourceGroupSettingsCppEmpty>(m_settings))
	{
		compilerFlags.push_back(L"-x");
		compilerFlags.push_back(L"c++");
	}

	{
		// Add the source paths as HeaderSearchPaths as well, so clang will also look here when searching include files.
		std::vector<FilePath> indexedDirectoryPaths;
		for (const FilePath& sourcePath : indexedPaths)
		{
			if (sourcePath.isDirectory())
			{
				indexedDirectoryPaths.push_back(sourcePath);
			}
		}

		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForSystemHeaderSearchPaths(
			utility::concat(indexedDirectoryPaths, utility::concat(m_settings->getHeaderSearchPathsExpandedAndAbsolute(), appSettings->getHeaderSearchPathsExpanded()))));
	}
	{
		utility::append(compilerFlags, IndexerCommandCxx::getCompilerFlagsForFrameworkSearchPaths(
			utility::concat(m_settings->getFrameworkSearchPathsExpandedAndAbsolute(), appSettings->getFrameworkSearchPathsExpanded())));
	}

	utility::append(compilerFlags, m_settings->getCompilerFlags());

	return compilerFlags;
}

