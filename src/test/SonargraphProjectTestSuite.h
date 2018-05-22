#include "cxxtest/TestSuite.h"

#include <fstream>

#include "data/indexer/IndexerCommandCxxEmpty.h"
#include "data/indexer/IndexerCommandJava.h"
#include "project/SourceGroupJavaSonargraph.h"
#include "settings/SourceGroupSettingsCxxSonargraph.h"
#include "settings/SourceGroupSettingsJavaSonargraph.h"
#include "settings/ProjectSettings.h"
#include "settings/ApplicationSettings.h"
#include "utility/sonargraph/SonargraphProject.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"

class SonargraphProjectTestSuite: public CxxTest::TestSuite
{
public:
	static const bool s_updateExpectedOutput = false;

	void test_sonargraph_project_with_cmake_json_modules_generates_expected_output()
	{
		const std::wstring projectName = L"CxxCmakeJsonModules";

		ProjectSettings projectSettings;
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxxSonargraph>("fake_id", &projectSettings);

		std::shared_ptr<ApplicationSettings> applicationSettings = std::make_shared<ApplicationSettings>();
		applicationSettings->setHeaderSearchPaths({ FilePath(L"test/header/search/path") });
		applicationSettings->setFrameworkSearchPaths({ FilePath(L"test/framework/search/path") });

		generateAndCompareExpectedOutputForSonargraphProject(projectName, sourceGroupSettings, applicationSettings);
	}

	void test_sonargraph_project_with_java_modules_generates_expected_output()
	{
		const std::wstring projectName = L"JavaModules";

		ProjectSettings projectSettings;
		std::shared_ptr<SourceGroupSettingsJavaSonargraph> sourceGroupSettings =
			std::make_shared<SourceGroupSettingsJavaSonargraph>("fake_id", &projectSettings);
		sourceGroupSettings->setUseJreSystemLibrary(true);
		sourceGroupSettings->setClasspath({
			FilePath(L"test/classpath/file.jar"), FilePath(L"test/classpath/dir")
		});

		std::shared_ptr<ApplicationSettings> applicationSettings = std::make_shared<ApplicationSettings>();
		applicationSettings->setJreSystemLibraryPaths({ FilePath(L"test/jre/system/library/path.jar") });

		generateAndCompareExpectedOutputForSonargraphProject(projectName, sourceGroupSettings, applicationSettings);
	}

	void test_sonargraph_project_with_cpp_modules_does_not_generate_output_for_java_sourcegroup()
	{
		const std::wstring projectName = L"CxxCmakeJsonModules";

		ProjectSettings projectSettings;
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings = std::make_shared<SourceGroupSettingsJavaSonargraph>("fake_id", &projectSettings);
		std::shared_ptr<ApplicationSettings> applicationSettings = std::make_shared<ApplicationSettings>();

		std::shared_ptr<TextAccess> output = generateExpectedOutputForSonargraphProject(projectName, sourceGroupSettings, applicationSettings);

		TS_ASSERT_EQUALS(0, output->getLineCount());
	}

	void test_sonargraph_project_with_java_modules_does_not_generate_output_for_cpp_sourcegroup()
	{
		const std::wstring projectName = L"JavaModules";

		ProjectSettings projectSettings;
		std::shared_ptr<SourceGroupSettings> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxxSonargraph>("fake_id", &projectSettings);
		std::shared_ptr<ApplicationSettings> applicationSettings = std::make_shared<ApplicationSettings>();

		std::shared_ptr<TextAccess> output = generateExpectedOutputForSonargraphProject(projectName, sourceGroupSettings, applicationSettings);

		TS_ASSERT_EQUALS(0, output->getLineCount());
	}

	void generateAndCompareExpectedOutputForSonargraphProject(
		std::wstring projectName,
		std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
		std::shared_ptr<const ApplicationSettings> applicationSettings)
	{
		const FilePath projectDataRoot = FilePath(L"data/SonargraphProjectTestSuite/" + projectName).makeAbsolute();
		const FilePath projectDataExpectedOutputRoot = projectDataRoot.getConcatenated(L"expected_output");

		std::shared_ptr<TextAccess> output = generateExpectedOutputForSonargraphProject(projectName, sourceGroupSettings, applicationSettings);

		FilePath expectedOutputFilePath = projectDataExpectedOutputRoot.getConcatenated(L"output.txt");
		if (s_updateExpectedOutput || !expectedOutputFilePath.exists())
		{
			std::ofstream expectedOutputFile;
			expectedOutputFile.open(expectedOutputFilePath.str());
			expectedOutputFile << output->getText();
			expectedOutputFile.close();
		}
		else
		{
			std::shared_ptr<TextAccess> expectedOutput = TextAccess::createFromFile(expectedOutputFilePath);
			TSM_ASSERT_EQUALS(L"Output does not match the expected line count for project \"" + projectName + L"\".", expectedOutput->getLineCount(), output->getLineCount());
			if (expectedOutput->getLineCount() == output->getLineCount())
			{
				for (size_t i = 1; i <= expectedOutput->getLineCount(); i++)
				{
					TS_ASSERT_EQUALS(expectedOutput->getLine(i), output->getLine(i));
				}
			}
		}
	}

	std::shared_ptr<TextAccess> generateExpectedOutputForSonargraphProject(
		std::wstring projectName,
		std::shared_ptr<const SourceGroupSettings> sourceGroupSettings,
		std::shared_ptr<const ApplicationSettings> applicationSettings)
	{
		const FilePath projectDataRoot = FilePath(L"data/SonargraphProjectTestSuite/" + projectName).makeAbsolute();
		const FilePath projectDataSrcRoot = projectDataRoot.getConcatenated(L"src/sonargraph");

		std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(projectDataSrcRoot.getConcatenated(L"system.sonargraph"), sourceGroupSettings->getLanguage());
		TS_ASSERT(project.use_count() > 0);

		std::vector<std::shared_ptr<IndexerCommand>> indexerCommands = project->getIndexerCommands(sourceGroupSettings, applicationSettings);

		std::wstring outputString;
		for (std::shared_ptr<IndexerCommand> indexerCommand : indexerCommands)
		{
			outputString += indexerCommandToString(indexerCommand, projectDataRoot);
		}

		return TextAccess::createFromString(utility::encodeToUtf8(outputString));
	}

	std::wstring indexerCommandToString(std::shared_ptr<const IndexerCommand> indexerCommand, const FilePath& baseDirectory)
	{
		if (indexerCommand)
		{
			if (std::shared_ptr<const IndexerCommandCxxEmpty> indexerCommandCxxEmpty = std::dynamic_pointer_cast<const IndexerCommandCxxEmpty>(indexerCommand))
			{
				return indexerCommandCxxEmptyToString(indexerCommandCxxEmpty, baseDirectory);
			}
			if (std::shared_ptr<const IndexerCommandJava> indexerCommandJava = std::dynamic_pointer_cast<const IndexerCommandJava>(indexerCommand))
			{
				return indexerCommandJavaToString(indexerCommandJava, baseDirectory);
			}
			return L"Unsupported indexer command type: " + utility::decodeFromUtf8(indexerCommandTypeToString(indexerCommand->getIndexerCommandType()));
		}
		return L"No IndexerCommand provided.";
	}

	std::wstring indexerCommandCxxEmptyToString(std::shared_ptr<const IndexerCommandCxxEmpty> indexerCommand, const FilePath& baseDirectory)
	{
		std::wstring result;
		result += L"SourceFilePath: \"" + indexerCommand->getSourceFilePath().getRelativeTo(baseDirectory).wstr() + L"\"\n";
		result += L"\tLanguageStandard: \"" + utility::decodeFromUtf8(indexerCommand->getLanguageStandard()) + L"\"\n";
		for (const FilePath& indexedPath : indexerCommand->getIndexedPaths())
		{
			result += L"\tIndexedPath: \"" + indexedPath.getRelativeTo(baseDirectory).wstr() + L"\"\n";
		}
		for (const std::wstring& compilerFlag : indexerCommand->getCompilerFlags())
		{
			result += L"\tCompilerFlag: \"" + compilerFlag + L"\"\n";
		}
		for (const FilePath& headerSearchPath : indexerCommand->getSystemHeaderSearchPaths())
		{
			result += L"\tHeaderSearchPath: \"" + headerSearchPath.getRelativeTo(baseDirectory).wstr() + L"\"\n";
		}
		for (const FilePath& frameworkSearchPath : indexerCommand->getFrameworkSearchPaths())
		{
			result += L"\tFrameworkSearchPath: \"" + frameworkSearchPath.wstr() + L"\"\n";
		}
		return result;
	}

	std::wstring indexerCommandJavaToString(std::shared_ptr<const IndexerCommandJava> indexerCommand, const FilePath& baseDirectory)
	{
		std::wstring result;
		result += L"SourceFilePath: \"" + indexerCommand->getSourceFilePath().getRelativeTo(baseDirectory).wstr() + L"\"\n";
		result += L"\tLanguageStandard: \"" + utility::decodeFromUtf8(indexerCommand->getLanguageStandard()) + L"\"\n";
		for (const FilePath& classPathItem : indexerCommand->getClassPath())
		{
			result += L"\tClassPathItem: \"" + classPathItem.getRelativeTo(baseDirectory).wstr() + L"\"\n";
		}
		return result;
	}
};
