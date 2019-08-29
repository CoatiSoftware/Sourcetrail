#include "utilitySourceGroupCxx.h"

#include <clang/Tooling/JSONCompilationDatabase.h>

#include "CanonicalFilePathCache.h"
#include "CxxCompilationDatabaseSingle.h"
#include "CxxDiagnosticConsumer.h"
#include "CxxParser.h"
#include "DialogView.h"
#include "FilePathFilter.h"
#include "FileRegister.h"
#include "FileSystem.h"
#include "GeneratePCHAction.h"
#include "logging.h"
#include "ParserClientImpl.h"
#include "SingleFrontendActionFactory.h"
#include "SourceGroupSettingsWithCxxPchOptions.h"
#include "StorageProvider.h"
#include "TaskLambda.h"
#include "utility.h"

namespace utility
{
	std::shared_ptr<Task> createBuildPchTask(
		const SourceGroupSettingsWithCxxPchOptions* settings,
		std::vector<std::wstring> compilerFlags,
		std::shared_ptr<StorageProvider> storageProvider,
		std::shared_ptr<DialogView> dialogView)
	{
		FilePath pchInputFilePath = settings->getPchInputFilePathExpandedAndAbsolute();
		FilePath pchDependenciesDirectoryPath = settings->getPchDependenciesDirectoryPath();

		if (pchInputFilePath.empty() || pchDependenciesDirectoryPath.empty())
		{
			return std::make_shared<TaskLambda>([](){});
		}

		if (!pchInputFilePath.exists())
		{
			LOG_ERROR(L"Precompiled header input file \"" + pchInputFilePath.wstr() + L"\" does not exist.");
			return std::make_shared<TaskLambda>([]() {});
		}

		const FilePath pchOutputFilePath =
			pchDependenciesDirectoryPath.getConcatenated(pchInputFilePath.fileName()).replaceExtension(L"pch");

		utility::removeIncludePchFlag(compilerFlags);
		compilerFlags.push_back(pchInputFilePath.wstr());
		compilerFlags.push_back(L"-emit-pch");
		compilerFlags.push_back(L"-o");
		compilerFlags.push_back(pchOutputFilePath.wstr());

		return std::make_shared<TaskLambda>(
			[dialogView, storageProvider, pchInputFilePath, pchOutputFilePath, compilerFlags]()
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

				std::shared_ptr<IntermediateStorage> storage = std::make_shared<IntermediateStorage>();
				std::shared_ptr<ParserClientImpl> client = std::make_shared<ParserClientImpl>(storage.get());

				std::shared_ptr<FileRegister> fileRegister = std::make_shared<FileRegister>(
					pchInputFilePath, std::set<FilePath>{ pchInputFilePath }, std::set<FilePathFilter>{});

				std::shared_ptr<CanonicalFilePathCache> canonicalFilePathCache =
					std::make_shared<CanonicalFilePathCache>(fileRegister);

				clang::tooling::CompileCommand pchCommand;
				pchCommand.Filename = utility::encodeToUtf8(pchInputFilePath.fileName());
				pchCommand.Directory = pchOutputFilePath.getParentDirectory().str();
				// DON'T use "-fsyntax-only" here because it will cause the output file to be erased
				pchCommand.CommandLine =
					utility::concat({ "clang-tool" }, CxxParser::getCommandlineArgumentsEssential(compilerFlags));

				CxxCompilationDatabaseSingle compilationDatabase(pchCommand);
				clang::tooling::ClangTool tool(compilationDatabase, { utility::encodeToUtf8(pchInputFilePath.wstr()) });
				GeneratePCHAction* action = new GeneratePCHAction(client, canonicalFilePathCache);

				llvm::IntrusiveRefCntPtr<clang::DiagnosticOptions> options = new clang::DiagnosticOptions();
				CxxDiagnosticConsumer diagnostics(
					llvm::errs(), &*options, client, canonicalFilePathCache, pchInputFilePath, true);

				tool.setDiagnosticConsumer(&diagnostics);
				tool.clearArgumentsAdjusters();
				tool.run(new SingleFrontendActionFactory(action));

				storageProvider->insert(storage);
			}
		);
	}

	bool containsIncludePchFlags(std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb)
	{
		for (const clang::tooling::CompileCommand& command : cdb->getAllCompileCommands())
		{
			if (containsIncludePchFlag(command.CommandLine))
			{
				return true;
			}
		}
		return false;
	}

	bool containsIncludePchFlag(const std::vector<std::string>& args)
	{
		const std::string includePchPrefix = "-include-pch";
		for (size_t i = 0; i < args.size(); i++)
		{
			const std::string arg = utility::trim(args[i]);
			if (utility::isPrefix(includePchPrefix, arg))
			{
				return true;
			}
		}
		return false;
	}

	std::vector<std::wstring> getWithRemoveIncludePchFlag(const std::vector<std::wstring>& args)
	{
		std::vector<std::wstring> ret = args;
		removeIncludePchFlag(ret);
		return ret;
	}

	void removeIncludePchFlag(std::vector<std::wstring>& args)
	{
		const std::wstring includePchPrefix = L"-include-pch";
		for (size_t i = 0; i < args.size(); i++)
		{
			const std::wstring arg = utility::trim(args[i]);
			if (utility::isPrefix<std::wstring>(includePchPrefix, arg))
			{
				if (i + 1 < args.size() &&
					!utility::isPrefix<std::wstring>(L"-", utility::trim(args[i + 1])) &&
					arg == includePchPrefix)
				{
					args.erase(args.begin() + i + 1);
				}
				args.erase(args.begin() + i);
				i--;
			}
		}
	}

	std::vector<std::wstring> getIncludePchFlags(const SourceGroupSettingsWithCxxPchOptions* settings)
	{
		const FilePath pchInputFilePath = settings->getPchInputFilePathExpandedAndAbsolute();
		const FilePath pchDependenciesDirectoryPath = settings->getPchDependenciesDirectoryPath();

		if (!pchInputFilePath.empty() && !pchDependenciesDirectoryPath.empty())
		{
			const FilePath pchOutputFilePath =
				pchDependenciesDirectoryPath.getConcatenated(pchInputFilePath.fileName()).replaceExtension(L"pch");

			return {
				L"-fallow-pch-with-compiler-errors",
				L"-include-pch",
				pchOutputFilePath.wstr()
			};
		}

		return {};
	}
}
