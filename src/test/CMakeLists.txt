add_files(
	TEST

	helper/TestFileRegister.cpp
	helper/TestFileRegister.h
	helper/TestIntermediateStorage.h

	TestSuiteFixture.cpp
	TestSuiteFixture.h

	CommandlineTestSuite.h
	ConfigManagerTestSuite.h
	CxxIncludeProcessingTestSuite.h
	CxxIndexSampleProjectsTestSuite.h
	CxxParserTestSuite.h
	CxxTypeNameTestSuite.h
	FileManagerTestSuite.h
	FilePathFilterTestSuite.h
	FilePathTestSuite.h
	FileSystemTestSuite.h
	GeneratorTestSuite.h
	GraphTestSuite.h
	LogManagerTestSuite.h
	LowMemoryStringMapTestSuite.h
	MatrixBaseTestSuite.h
	MessageQueueTestSuite.h
	NetworkProtocolHelperTestSuite.h
	RefreshInfoGeneratorTestSuite.h
	SearchIndexTestSuite.h
	SettingsMigratorTestSuite.h
	SettingsTestSuite.h
	SharedMemoryTestSuite.h
	SourceGroupTestSuite.h
	SourceLocationCollectionTestSuite.h
	SqliteBookmarkStorageTestSuite.h
	SqliteIndexStorageTestSuite.h
	StorageTestSuite.h
	TaskSchedulerTestSuite.h
	TextAccessTestSuite.h
	UtilityMavenTestSuite.h
	UtilityStringTestSuite.h
	UtilityTestSuite.h
	Vector2TestSuite.h

# Java tests need to be executed last because of some linux related issues.
	JavaParserTestSuite.h
	JavaIndexSampleProjectsTestSuite.h
)
