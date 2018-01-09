add_files(
	TEST_FILES

	helper/DumpParserClient.h
	helper/TestFileRegister.cpp
	helper/TestFileRegister.h
	helper/TestParserClient.h

	TestSuiteFixture.cpp
	TestSuiteFixture.h

	CommandlineTestSuite.h
	ConfigManagerTestSuite.h
	CxxIncludeProcessingTestSuite.h
	CxxIndexSampleProjectsTestSuite.h
	CxxParserTestSuite.h
	CxxTypeNameTestSuite.h
	FileManagerTestSuite.h
	FilePathTestSuite.h
	FileSystemTestSuite.h
	GeneratorTestSuite.h
	GraphTestSuite.h
	LogManagerTestSuite.h
	MatrixBaseTestSuite.h
	MessageQueueTestSuite.h
	NetworkProtocolHelperTestSuite.h
    SettingsTestSuite.h
	SettingsMigratorTestSuite.h
	SearchIndexTestSuite.h
	SharedMemoryTestSuite.h
	SourceLocationCollectionTestSuite.h
	SqliteBookmarkStorageTestSuite.h
	SqliteIndexStorageTestSuite.h
	StorageTestSuite.h
	TaskSchedulerTestSuite.h
	TextAccessTestSuite.h
	UtilityMavenTestSuite.h
	UtilityStringTestSuite.h
	Vector2TestSuite.h
	
# Java tests need to be executed last because of some linux related issues.
    JavaParserTestSuite.h
	JavaIndexSampleProjectsTestSuite.h
)
