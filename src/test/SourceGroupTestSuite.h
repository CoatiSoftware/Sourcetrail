#include <cxxtest/TestSuite.h>

#include <fstream>

#include "IndexerCommandCxx.h"
#include "IndexerCommandJava.h"
#include "JavaEnvironmentFactory.h"
#include "SourceGroupCxxEmpty.h"
#include "SourceGroupCxxCdb.h"
#include "SourceGroupCxxCodeblocks.h"
#include "SourceGroupCxxSonargraph.h"
#include "SourceGroupJavaEmpty.h"
#include "SourceGroupJavaGradle.h"
#include "SourceGroupJavaMaven.h"
#include "SourceGroupJavaSonargraph.h"
#include "SourceGroupSettingsCEmpty.h"
#include "SourceGroupSettingsCppEmpty.h"
#include "SourceGroupSettingsCxxCdb.h"
#include "SourceGroupSettingsCxxCodeblocks.h"
#include "SourceGroupSettingsCxxSonargraph.h"
#include "SourceGroupSettingsJavaEmpty.h"
#include "SourceGroupSettingsJavaGradle.h"
#include "SourceGroupSettingsJavaMaven.h"
#include "SourceGroupSettingsJavaSonargraph.h"
#include "ProjectSettings.h"
#include "ApplicationSettings.h"
#include "FileSystem.h"
#include "TextAccess.h"
#include "AppPath.h"
#include "utilityJava.h"
#include "utilityPathDetection.h"
#include "utilityString.h"
#include "Version.h"
#include "Application.h"

class SourceGroupTestSuite: public CxxTest::TestSuite
{
public:
	static const bool s_updateExpectedOutput = false;

	void test_finds_all_jar_dependencies()
	{
		for (const std::wstring& jarName : utility::getRequiredJarNames())
		{
			FilePath jarPath = FilePath(L"../app/data/java/lib/").concatenate(jarName);
			TSM_ASSERT(L"Jar dependency path does not exist: " + jarPath.wstr(), jarPath.exists());
		}
	}

	void test_can_setup_environment_factory()
	{
		std::vector<FilePath> javaPaths = utility::getJavaRuntimePathDetector()->getPaths();
		if (!javaPaths.empty())
		{
			ApplicationSettings::getInstance()->setJavaPath(javaPaths[0]);
		}

		const std::string errorString = setupJavaEnvironmentFactory();

		TS_ASSERT_EQUALS("", errorString);

		// if this one fails, maybe your java_path in the test settings is wrong.
		TS_ASSERT_LESS_THAN_EQUALS(1, JavaEnvironmentFactory::getInstance().use_count());
	}

	void test_can_create_application_instance()
	{
		// required to query in SourceGroup for dialog view... this is not a very elegant solution. should be refactored to pass dialog view to SourceGroup on creation.
		Application::createInstance(Version(), nullptr, nullptr);
		TS_ASSERT_LESS_THAN_EQUALS(1, Application::getInstance().use_count());
	}

	void test_source_group_cxx_c_empty_generates_expected_output()
	{
		const std::wstring projectName = L"cxx_c_empty";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsCEmpty> sourceGroupSettings = std::make_shared<SourceGroupSettingsCEmpty>("fake_id", &projectSettings);
		sourceGroupSettings->setSourcePaths({ getInputDirectoryPath(projectName).concatenate(L"src") });
		sourceGroupSettings->setSourceExtensions({ L".c" });
		sourceGroupSettings->setExcludeFilterStrings({ L"**/excluded/**" });
		sourceGroupSettings->setTargetOptionsEnabled(true);
		sourceGroupSettings->setTargetArch(L"test_arch");
		sourceGroupSettings->setTargetVendor(L"test_vendor");
		sourceGroupSettings->setTargetSys(L"test_sys");
		sourceGroupSettings->setTargetAbi(L"test_abi");
		sourceGroupSettings->setCStandard(L"c11");
		sourceGroupSettings->setHeaderSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"header_search/local") });
		sourceGroupSettings->setFrameworkSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"framework_search/local") });
		sourceGroupSettings->setCompilerFlags({ L"-local-flag" });

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedHeaderSearchPaths = applicationSettings->getHeaderSearchPaths();
		std::vector<FilePath> storedFrameworkSearchPaths = applicationSettings->getFrameworkSearchPaths();

		applicationSettings->setHeaderSearchPaths({ FilePath(L"test/header/search/path") });
		applicationSettings->setFrameworkSearchPaths({ FilePath(L"test/framework/search/path") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupCxxEmpty>(sourceGroupSettings));

		applicationSettings->setHeaderSearchPaths(storedHeaderSearchPaths);
		applicationSettings->setFrameworkSearchPaths(storedFrameworkSearchPaths);
	}

	void test_source_group_cxx_cpp_empty_generates_expected_output()
	{
		const std::wstring projectName = L"cxx_cpp_empty";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsCppEmpty> sourceGroupSettings = std::make_shared<SourceGroupSettingsCppEmpty>("fake_id", &projectSettings);
		sourceGroupSettings->setSourcePaths({ getInputDirectoryPath(projectName).concatenate(L"/src") });
		sourceGroupSettings->setSourceExtensions({ L".cpp" });
		sourceGroupSettings->setExcludeFilterStrings({ L"**/excluded/**" });
		sourceGroupSettings->setTargetOptionsEnabled(true);
		sourceGroupSettings->setTargetArch(L"test_arch");
		sourceGroupSettings->setTargetVendor(L"test_vendor");
		sourceGroupSettings->setTargetSys(L"test_sys");
		sourceGroupSettings->setTargetAbi(L"test_abi");
		sourceGroupSettings->setCppStandard(L"c++11");
		sourceGroupSettings->setHeaderSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"header_search/local") });
		sourceGroupSettings->setFrameworkSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"framework_search/local") });
		sourceGroupSettings->setCompilerFlags({ L"-local-flag" });

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedHeaderSearchPaths = applicationSettings->getHeaderSearchPaths();
		std::vector<FilePath> storedFrameworkSearchPaths = applicationSettings->getFrameworkSearchPaths();

		applicationSettings->setHeaderSearchPaths({ FilePath(L"test/header/search/path") });
		applicationSettings->setFrameworkSearchPaths({ FilePath(L"test/framework/search/path") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupCxxEmpty>(sourceGroupSettings));

		applicationSettings->setHeaderSearchPaths(storedHeaderSearchPaths);
		applicationSettings->setFrameworkSearchPaths(storedFrameworkSearchPaths);
	}

	void test_source_group_cxx_codeblocks_generates_expected_output()
	{
		const std::wstring projectName = L"cxx_codeblocks";
		const FilePath cbpPath = getInputDirectoryPath(projectName).concatenate(L"project.cbp");
		const FilePath sourceCbpPath = getInputDirectoryPath(projectName).concatenate(L"project.cbp.in");

		FileSystem::remove(cbpPath);

		{
			std::ofstream fileStream;
			fileStream.open(cbpPath.str(), std::ios::app);
			fileStream << utility::replace(
				TextAccess::createFromFile(sourceCbpPath)->getText(), "<source_path>", getInputDirectoryPath(projectName).concatenate(L"src").getAbsolute().str()
			);
			fileStream.close();
		}

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsCxxCodeblocks> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxxCodeblocks>("fake_id", &projectSettings);
		sourceGroupSettings->setCodeblocksProjectPath(cbpPath);
		sourceGroupSettings->setCppStandard(L"c++11");
		sourceGroupSettings->setCStandard(L"c11");
		sourceGroupSettings->setExcludeFilterStrings({ L"**/excluded/**" });
		sourceGroupSettings->setIndexedHeaderPaths({ FilePath(L"test/indexed/header/path") });
		sourceGroupSettings->setSourceExtensions({ L".cpp", L".c" });
		sourceGroupSettings->setHeaderSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"header_search/local") });
		sourceGroupSettings->setFrameworkSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"framework_search/local") });
		sourceGroupSettings->setCompilerFlags({ L"-local-flag" });

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedHeaderSearchPaths = applicationSettings->getHeaderSearchPaths();
		std::vector<FilePath> storedFrameworkSearchPaths = applicationSettings->getFrameworkSearchPaths();

		applicationSettings->setHeaderSearchPaths({ FilePath(L"test/header/search/path") });
		applicationSettings->setFrameworkSearchPaths({ FilePath(L"test/framework/search/path") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupCxxCodeblocks>(sourceGroupSettings));

		applicationSettings->setHeaderSearchPaths(storedHeaderSearchPaths);
		applicationSettings->setFrameworkSearchPaths(storedFrameworkSearchPaths);

		FileSystem::remove(cbpPath);
	}

	void test_source_group_cxx_cdb_generates_expected_output()
	{
		const std::wstring projectName = L"cxx_cdb";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsCxxCdb> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxxCdb>("fake_id", &projectSettings);
		sourceGroupSettings->setIndexedHeaderPaths({ FilePath(L"test/indexed/header/path") });
		sourceGroupSettings->setCompilationDatabasePath(getInputDirectoryPath(projectName).concatenate(L"compile_commands.json"));
		sourceGroupSettings->setExcludeFilterStrings({ L"**/excluded/**" });
		sourceGroupSettings->setHeaderSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"header_search/local") });
		sourceGroupSettings->setFrameworkSearchPaths({ getInputDirectoryPath(projectName).concatenate(L"framework_search/local") });
		sourceGroupSettings->setCompilerFlags({ L"-local-flag" });

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedHeaderSearchPaths = applicationSettings->getHeaderSearchPaths();
		std::vector<FilePath> storedFrameworkSearchPaths = applicationSettings->getFrameworkSearchPaths();

		applicationSettings->setHeaderSearchPaths({ FilePath(L"test/header/search/path") });
		applicationSettings->setFrameworkSearchPaths({ FilePath(L"test/framework/search/path") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupCxxCdb>(sourceGroupSettings));

		applicationSettings->setHeaderSearchPaths(storedHeaderSearchPaths);
		applicationSettings->setFrameworkSearchPaths(storedFrameworkSearchPaths);
	}

	void test_source_group_cxx_sonargraph_with_cmake_json_modules_generates_expected_output()
	{
		const std::wstring projectName = L"cxx_sonargraph_cmake_json";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsCxxSonargraph> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxxSonargraph>("fake_id", &projectSettings);
		sourceGroupSettings->setIndexedHeaderPaths({ FilePath(L"test/indexed/header/path") });
		sourceGroupSettings->setSonargraphProjectPath(getInputDirectoryPath(projectName).concatenate(L"sonargraph/system.sonargraph"));

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedHeaderSearchPaths = applicationSettings->getHeaderSearchPaths();
		std::vector<FilePath> storedFrameworkSearchPaths = applicationSettings->getFrameworkSearchPaths();

		applicationSettings->setHeaderSearchPaths({ FilePath(L"test/header/search/path") });
		applicationSettings->setFrameworkSearchPaths({ FilePath(L"test/framework/search/path") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupCxxSonargraph>(sourceGroupSettings));

		applicationSettings->setHeaderSearchPaths(storedHeaderSearchPaths);
		applicationSettings->setFrameworkSearchPaths(storedFrameworkSearchPaths);
	}

	void test_source_group_cxx_sonargraph_with_cpp_manual_modules_generates_expected_output()
	{
		const std::wstring projectName = L"cxx_sonargraph_cpp_manual";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsCxxSonargraph> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxxSonargraph>("fake_id", &projectSettings);
		sourceGroupSettings->setIndexedHeaderPaths({ FilePath(L"test/indexed/header/path") });
		sourceGroupSettings->setSonargraphProjectPath(getInputDirectoryPath(projectName).concatenate(L"/sonargraph/system.sonargraph"));

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedHeaderSearchPaths = applicationSettings->getHeaderSearchPaths();
		std::vector<FilePath> storedFrameworkSearchPaths = applicationSettings->getFrameworkSearchPaths();

		applicationSettings->setHeaderSearchPaths({ FilePath(L"test/header/search/path") });
		applicationSettings->setFrameworkSearchPaths({ FilePath(L"test/framework/search/path") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupCxxSonargraph>(sourceGroupSettings));

		applicationSettings->setHeaderSearchPaths(storedHeaderSearchPaths);
		applicationSettings->setFrameworkSearchPaths(storedFrameworkSearchPaths);
	}

	void test_sourcegroup_java_empty_generates_expected_output()
	{
		const std::wstring projectName = L"java_empty";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsJavaEmpty> sourceGroupSettings =
			std::make_shared<SourceGroupSettingsJavaEmpty>("fake_id", &projectSettings);
		sourceGroupSettings->setSourceExtensions({ L".java" });
		sourceGroupSettings->setExcludeFilterStrings({ L"**/Foo.java" });
		sourceGroupSettings->setJavaStandard({ L"10" });
		sourceGroupSettings->setSourcePaths({ getInputDirectoryPath(projectName).concatenate(L"src") });
		sourceGroupSettings->setUseJreSystemLibrary(true);
		sourceGroupSettings->setClasspath({
			getInputDirectoryPath(projectName).concatenate(L"lib/dependency.jar"), getInputDirectoryPath(projectName).concatenate(L"classpath_dir")
		});

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedJreSystemLibraryPaths = applicationSettings->getJreSystemLibraryPaths();

		applicationSettings->setJreSystemLibraryPaths({ FilePath(L"test/jre/system/library/path.jar") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupJavaEmpty>(sourceGroupSettings));

		applicationSettings->setJreSystemLibraryPaths(storedJreSystemLibraryPaths);
	}

	void test_sourcegroup_java_gradle_generates_expected_output()
	{
		const std::wstring projectName = L"java_gradle";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsJavaGradle> sourceGroupSettings =
			std::make_shared<SourceGroupSettingsJavaGradle>("fake_id", &projectSettings);
		sourceGroupSettings->setSourceExtensions({ L".java" });
		sourceGroupSettings->setExcludeFilterStrings({ L"**/HelloWorld.java" });
		sourceGroupSettings->setJavaStandard({ L"10" });
		sourceGroupSettings->setGradleProjectFilePath({ getInputDirectoryPath(projectName).concatenate(L"build.gradle") });
		sourceGroupSettings->setShouldIndexGradleTests(true);
		sourceGroupSettings->setGradleDependenciesDirectory(getInputDirectoryPath(projectName).concatenate(L"gradle_dependencies"));

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		const FilePath storedAppPath = AppPath::getAppPath();
		AppPath::setAppPath(storedAppPath.getConcatenated(L"../app").makeAbsolute());

		std::vector<FilePath> storedJreSystemLibraryPaths = applicationSettings->getJreSystemLibraryPaths();
		applicationSettings->setJreSystemLibraryPaths({ FilePath(L"test/jre/system/library/path.jar") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupJavaGradle>(sourceGroupSettings));

		applicationSettings->setJreSystemLibraryPaths(storedJreSystemLibraryPaths);
		AppPath::setAppPath(storedAppPath);
	}

	void test_sourcegroup_java_maven_generates_expected_output()
	{
		std::vector<FilePath> mavenPaths = utility::getMavenExecutablePathDetector()->getPaths();

		TS_ASSERT(!mavenPaths.empty());

		if (!mavenPaths.empty())
		{
			ApplicationSettings::getInstance()->setMavenPath(mavenPaths.front());
		}

		const std::wstring projectName = L"java_maven";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsJavaMaven> sourceGroupSettings =
			std::make_shared<SourceGroupSettingsJavaMaven>("fake_id", &projectSettings);
		sourceGroupSettings->setSourceExtensions({ L".java" });
		sourceGroupSettings->setExcludeFilterStrings({ L"**/Foo.java" });
		sourceGroupSettings->setJavaStandard({ L"10" });
		sourceGroupSettings->setMavenProjectFilePath({ getInputDirectoryPath(projectName).concatenate(L"my-app/pom.xml") });
		sourceGroupSettings->setShouldIndexMavenTests(true);
		sourceGroupSettings->setMavenDependenciesDirectory(getInputDirectoryPath(projectName).concatenate(L"maven_dependencies"));

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedJreSystemLibraryPaths = applicationSettings->getJreSystemLibraryPaths();

		applicationSettings->setJreSystemLibraryPaths({ FilePath(L"test/jre/system/library/path.jar") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupJavaMaven>(sourceGroupSettings));

		applicationSettings->setJreSystemLibraryPaths(storedJreSystemLibraryPaths);
	}

	void test_sourcegroup_java_sonargraph_with_java_modules_generates_expected_output()
	{
		const std::wstring projectName = L"java_sonargraph";

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsJavaSonargraph> sourceGroupSettings =
			std::make_shared<SourceGroupSettingsJavaSonargraph>("fake_id", &projectSettings);
		sourceGroupSettings->setUseJreSystemLibrary(true);
		sourceGroupSettings->setClasspath({
			FilePath(L"test/classpath/file.jar"), FilePath(L"test/classpath/dir")
		});
		sourceGroupSettings->setSonargraphProjectPath(getInputDirectoryPath(projectName).concatenate(L"sonargraph/system.sonargraph"));

		std::shared_ptr<ApplicationSettings> applicationSettings = ApplicationSettings::getInstance();

		std::vector<FilePath> storedJreSystemLibraryPaths = applicationSettings->getJreSystemLibraryPaths();

		applicationSettings->setJreSystemLibraryPaths({ FilePath(L"test/jre/system/library/path.jar") });

		generateAndCompareExpectedOutput(projectName, std::make_shared<SourceGroupJavaSonargraph>(sourceGroupSettings));

		applicationSettings->setJreSystemLibraryPaths(storedJreSystemLibraryPaths);
	}

	// Special Tests

	void test_sourcegroup_java_sonargraph_with_cpp_modules_does_not_generate_output()
	{
		const std::wstring projectName = L"cxx_sonargraph_cpp_manual";
		const FilePath sonargraphProjectFilePath(getInputDirectoryPath(projectName).concatenate(L"sonargraph/system.sonargraph"));

		TS_ASSERT(sonargraphProjectFilePath.exists());

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsJavaSonargraph> sourceGroupSettings = std::make_shared<SourceGroupSettingsJavaSonargraph>("fake_id", &projectSettings);
		sourceGroupSettings->setSonargraphProjectPath(sonargraphProjectFilePath);

		std::shared_ptr<TextAccess> output = generateExpectedOutput(projectName, std::make_shared<SourceGroupJavaSonargraph>(sourceGroupSettings));

		TS_ASSERT_EQUALS(0, output->getLineCount());
	}

	void test_sourcegroup_cxx_sonargraph_with_java_modules_does_not_generate_output()
	{
		const std::wstring projectName = L"java_sonargraph";
		const FilePath sonargraphProjectFilePath(getInputDirectoryPath(projectName).concatenate(L"sonargraph/system.sonargraph"));

		TS_ASSERT(sonargraphProjectFilePath.exists());

		ProjectSettings projectSettings;
		projectSettings.setProjectFilePath(L"non_existent_project", getInputDirectoryPath(projectName));

		std::shared_ptr<SourceGroupSettingsCxxSonargraph> sourceGroupSettings = std::make_shared<SourceGroupSettingsCxxSonargraph>("fake_id", &projectSettings);
		sourceGroupSettings->setSonargraphProjectPath(sonargraphProjectFilePath);

		std::shared_ptr<TextAccess> output = generateExpectedOutput(projectName, std::make_shared<SourceGroupCxxSonargraph>(sourceGroupSettings));

		TS_ASSERT_EQUALS(0, output->getLineCount());
	}

	void test_can_destroy_application_instance()
	{
		Application::destroyInstance();
		TS_ASSERT_EQUALS(0, Application::getInstance().use_count());
	}

	// Utility

private:
	static FilePath getInputDirectoryPath(const std::wstring& projectName)
	{
		return FilePath(L"data/SourceGroupTestSuite/" + projectName + L"/input").makeAbsolute();
	}

	static FilePath getOutputDirectoryPath(const std::wstring& projectName)
	{
		return FilePath(L"data/SourceGroupTestSuite/" + projectName + L"/expected_output").makeAbsolute();
	}

	std::string setupJavaEnvironmentFactory()
	{
		if (!JavaEnvironmentFactory::getInstance())
		{
			std::string errorString;
#ifdef _WIN32
			const std::string separator = ";";
#else
			const std::string separator = ":";
#endif
			std::string classPath = "";
			{
				const std::vector<std::wstring> jarNames = utility::getRequiredJarNames();
				for (size_t i = 0; i < jarNames.size(); i++)
				{
					if (i != 0)
					{
						classPath += separator;
					}
					classPath += FilePath(L"../app/data/java/lib/").concatenate(jarNames[i]).str();
				}
			}

			JavaEnvironmentFactory::createInstance(
				classPath,
				errorString
			);

			return errorString;
		}

		return "";
	}

	void generateAndCompareExpectedOutput(
		std::wstring projectName,
		std::shared_ptr<const SourceGroup> sourceGroup)
	{
		const std::shared_ptr<const TextAccess> output = generateExpectedOutput(projectName, sourceGroup);

		const FilePath expectedOutputFilePath = getOutputDirectoryPath(projectName).concatenate(L"output.txt");
		if (s_updateExpectedOutput || !expectedOutputFilePath.exists())
		{
			std::ofstream expectedOutputFile;
			expectedOutputFile.open(expectedOutputFilePath.str());
			expectedOutputFile << output->getText();
			expectedOutputFile.close();
		}
		else
		{
			const std::shared_ptr<const TextAccess> expectedOutput = TextAccess::createFromFile(expectedOutputFilePath);
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

	std::shared_ptr<TextAccess> generateExpectedOutput(
		std::wstring projectName,
		std::shared_ptr<const SourceGroup> sourceGroup)
	{
		const FilePath projectDataRoot = getInputDirectoryPath(projectName).makeAbsolute();

		std::vector<std::shared_ptr<IndexerCommand>> indexerCommands = sourceGroup->getIndexerCommands(sourceGroup->getAllSourceFilePaths());

		std::sort(
			indexerCommands.begin(),
			indexerCommands.end(),
			[](std::shared_ptr<IndexerCommand> a, std::shared_ptr<IndexerCommand> b)
			{
				return a->getSourceFilePath().wstr() < b->getSourceFilePath().wstr();
			}
		);

		std::wstring outputString;
		for (std::shared_ptr<IndexerCommand> indexerCommand : indexerCommands)
		{
			outputString += indexerCommandToString(indexerCommand, projectDataRoot);
		}

		return TextAccess::createFromString(utility::encodeToUtf8(outputString));
	}

	std::wstring indexerCommandToString(std::shared_ptr<IndexerCommand> indexerCommand, const FilePath& baseDirectory)
	{
		if (indexerCommand)
		{
			if (std::shared_ptr<const IndexerCommandCxx> indexerCommandCxx = std::dynamic_pointer_cast<const IndexerCommandCxx>(indexerCommand))
			{
				return indexerCommandCxxToString(indexerCommandCxx, baseDirectory);
			}
			if (std::shared_ptr<const IndexerCommandJava> indexerCommandJava = std::dynamic_pointer_cast<const IndexerCommandJava>(indexerCommand))
			{
				return indexerCommandJavaToString(indexerCommandJava, baseDirectory);
			}
			return L"Unsupported indexer command type: " + utility::decodeFromUtf8(indexerCommandTypeToString(indexerCommand->getIndexerCommandType()));
		}
		return L"No IndexerCommand provided.";
	}

	std::wstring indexerCommandCxxToString(std::shared_ptr<const IndexerCommandCxx> indexerCommand, const FilePath& baseDirectory)
	{
		std::wstring result;
		result += L"SourceFilePath: \"" + indexerCommand->getSourceFilePath().getRelativeTo(baseDirectory).wstr() + L"\"\n";
		for (const FilePath& indexedPath : indexerCommand->getIndexedPaths())
		{
			result += L"\tIndexedPath: \"" + indexedPath.getRelativeTo(baseDirectory).wstr() + L"\"\n";
		}
		for (std::wstring compilerFlag : indexerCommand->getCompilerFlags())
		{
			FilePath flagAsPath(compilerFlag);
			if (flagAsPath.exists())
			{
				compilerFlag = flagAsPath.getRelativeTo(baseDirectory).wstr();
			}
			result += L"\tCompilerFlag: \"" + compilerFlag + L"\"\n";
		}
		for (const FilePathFilter& filter : indexerCommand->getExcludeFilters())
		{
			result += L"\tExcludeFilter: \"" + filter.wstr() + L"\"\n";
		}
		return result;
	}

	std::wstring indexerCommandJavaToString(std::shared_ptr<const IndexerCommandJava> indexerCommand, const FilePath& baseDirectory)
	{
		std::wstring result;
		result += L"SourceFilePath: \"" + indexerCommand->getSourceFilePath().getRelativeTo(baseDirectory).wstr() + L"\"\n";
		result += L"\tLanguageStandard: \"" + indexerCommand->getLanguageStandard() + L"\"\n";
		for (const FilePath& classPathItem : indexerCommand->getClassPath())
		{
			result += L"\tClassPathItem: \"" + classPathItem.getRelativeTo(baseDirectory).wstr() + L"\"\n";
		}
		return result;
	}
};
