#include "utilitySourceGroupCxx.h"

#include "CxxCompilationDatabaseSingle.h"
#include "CxxParser.h"
#include "DialogView.h"
#include "FileSystem.h"
#include "GeneratePCHAction.h"
#include "logging.h"
#include "SingleFrontendActionFactory.h"
#include "SourceGroupSettingsCxx.h"
#include "SourceGroupSettingsWithCxxPchOptions.h"
#include "TaskLambda.h"
#include "utility.h"

namespace utility
{
	std::shared_ptr<Task> createBuildPchTask(
		const SourceGroupSettingsCxx* settings, std::vector<std::wstring> compilerFlags, std::shared_ptr<DialogView> dialogView)
	{
		const SourceGroupSettingsWithCxxPchOptions* pchSettings = dynamic_cast<const SourceGroupSettingsWithCxxPchOptions*>(settings);
		if (!pchSettings)
		{
			return std::make_shared<TaskLambda>([](){});
		}

		FilePath pchInputFilePath = pchSettings->getPchInputFilePathExpandedAndAbsolute();
		FilePath pchDependenciesDirectoryPath = pchSettings->getPchDependenciesDirectoryPath();

		if (pchInputFilePath.empty() || pchDependenciesDirectoryPath.empty())
		{
			return std::make_shared<TaskLambda>([](){});
		}

		if (!pchInputFilePath.exists())
		{
			LOG_ERROR(L"Precompiled header input file \"" + pchInputFilePath.wstr() + L"\" does not exist.");
			return std::make_shared<TaskLambda>([]() {});
		}

		const FilePath pchOutputFilePath = pchDependenciesDirectoryPath.getConcatenated(pchInputFilePath.fileName()).replaceExtension(L"pch");

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

	std::vector<std::wstring> getIncludePchFlags(const SourceGroupSettingsCxx* settings)
	{
		const SourceGroupSettingsWithCxxPchOptions* pchSettings = dynamic_cast<const SourceGroupSettingsWithCxxPchOptions*>(settings);
		if (pchSettings)
		{
			const FilePath pchInputFilePath = pchSettings->getPchInputFilePathExpandedAndAbsolute();
			const FilePath pchDependenciesDirectoryPath = pchSettings->getPchDependenciesDirectoryPath();

			if (!pchInputFilePath.empty() && !pchDependenciesDirectoryPath.empty())
			{
				const FilePath pchOutputFilePath = pchDependenciesDirectoryPath.getConcatenated(pchInputFilePath.fileName()).replaceExtension(L"pch");
				return {
					L"-fallow-pch-with-compiler-errors",
					L"-include-pch",
					pchOutputFilePath.wstr()
				};
			}
		}
		return {};
	}
}
