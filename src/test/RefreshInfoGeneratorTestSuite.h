#include <cxxtest/TestSuite.h>

#include "PersistentStorage.h"
#include "RefreshInfo.h"
#include "RefreshInfoGenerator.h"
#include "SourceGroup.h"
#include "ProjectSettings.h"
#include "SourceGroupSettings.h"
#include "FileSystem.h"
#include "utility.h"

class RefreshInfoGeneratorTestSuite: public CxxTest::TestSuite
{
public:
	FilePath m_indexDbPath = FilePath(L"data/RefreshInfoGeneratorTestSuite/project.srctrldb");
	FilePath m_bookmarkDbPath = FilePath(L"data/RefreshInfoGeneratorTestSuite/project.srctrlbm");
	FilePath m_sourceFolder = FilePath(L"data/RefreshInfoGeneratorTestSuite/src");

	void test_refresh_info_for_all_files_is_empty_for_empty_project()
	{
		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({})));

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForAllFiles(sourceGroups);

		TS_ASSERT_EQUALS(REFRESH_ALL_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_refresh_info_for_all_files_clears_nothing_and_indexes_previously_unknown_source_file()
	{
		cleanup();
		{
			const FilePath sourceFilePath = m_sourceFolder.getConcatenated(L"main.cpp");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({ sourceFilePath })));

			addFileToFileSystem(sourceFilePath);

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForAllFiles(sourceGroups);

			TS_ASSERT_EQUALS(REFRESH_ALL_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), sourceFilePath
			));
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_is_empty_for_empty_storage_and_empty_sourcegroup()
	{
		cleanup();
		{
			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
		}
		cleanup();
	}

	// Now we will test how the refresh info generator reacts to different situations when generating refresh info for updated
	// files. A file can have different states in the following dimensions:
	//	file may be known by the storage					unknown / nonindexed / indexed
	//	file may be changed									unchanged / changed
	//	file may be a source file							sourcefile / headerfile
	//	file may now be indexed by the source group			nottoindex / toindex
	//
	// We now wite a test case that checks for the expected result for every possible combination.
	//	Example: test_unknown_unchanged_sourcefile_that_is_nottoindex

	enum KnownState
	{
		UNKNOWN,
		NON_INDEXED,
		INDEXED
	};
	enum ChangedState
	{
		UNCHANGED,
		CHANGED
	};
	enum FileState
	{
		SOURCE_FILE,
		HEADER_FILE
	};
	enum IndexingState
	{
		NOT_TO_INDEX,
		TO_INDEX
	};

	RefreshInfo getRefreshInfo(KnownState knownState, ChangedState changedState, FileState fileState, IndexingState indexingState)
	{
		RefreshInfo refreshInfo;
		cleanup();
		{
			const FilePath filePath = m_sourceFolder.getConcatenated(L"file.extension");

			const std::set<FilePath> sourceFilePaths = ((fileState == SOURCE_FILE) ? std::set<FilePath>({ filePath }) : std::set<FilePath>({}));
			const std::set<FilePath> allFilePaths = { filePath };

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			if (indexingState == NOT_TO_INDEX)
			{
				// a file is "not existing" if the source group does not care about it
				sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({}, {})));
			}
			else // if (indexingState == TO_INDEX)
			{
				sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(sourceFilePaths, allFilePaths)));
			}

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			if (knownState == UNKNOWN)
			{
				// do not add anything to storage
			}
			else if (knownState == NON_INDEXED)
			{
				if (changedState == UNCHANGED)
				{
					addVeryNewFileToStorage(filePath, false, true, storage);
				}
				else // if (changedState == CHANGED)
				{
					addVeryOldFileToStorage(filePath, false, true, storage);
				}
			}
			else // if (knownState == INDEXED)
			{
				if (changedState == UNCHANGED)
				{
					addVeryNewFileToStorage(filePath, true, true, storage);
				}
				else // if (changedState == CHANGED)
				{
					addVeryOldFileToStorage(filePath, true, true, storage);
				}
			}

			addFileToFileSystem(filePath);

			storage->buildCaches();

			refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);
		}
		cleanup();

		return refreshInfo;
	}

	void test_unknown_unchanged_sourcefile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, SOURCE_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_unknown_unchanged_sourcefile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, SOURCE_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());
	}

	void test_unknown_unchanged_headerfile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, HEADER_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_unknown_unchanged_headerfile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, HEADER_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size()); // the header file will only be indexed on demand
	}

	void test_unknown_changed_sourcefile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, SOURCE_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_unknown_changed_sourcefile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, SOURCE_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());
	}

	void test_unknown_changed_headerfile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, HEADER_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_unknown_changed_headerfile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, HEADER_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size()); // the header file will only be indexed on demand
	}

	void test_nonindexed_unchanged_sourcefile_that_is_nottoindex() // this test does not really make sense
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, SOURCE_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_nonindexed_unchanged_sourcefile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, SOURCE_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());
	}

	void test_nonindexed_unchanged_headerfile_that_is_nottoindex() // this test does not make much sense without source files
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, HEADER_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_nonindexed_unchanged_headerfile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, HEADER_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size()); // the header file will only be indexed on demand
	}

	void test_nonindexed_changed_sourcefile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, SOURCE_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_nonindexed_changed_sourcefile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, SOURCE_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());
	}

	void test_nonindexed_changed_headerfile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, HEADER_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size()); // must be cleard here and will be re-indexed on demand
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_nonindexed_changed_headerfile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, HEADER_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size()); // the header file will only be indexed on demand
	}

	void test_indexed_unchanged_sourcefile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, SOURCE_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_indexed_unchanged_sourcefile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, SOURCE_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_indexed_unchanged_headerfile_that_is_nottoindex() // TODO: check if depending source file gets reindexed
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, HEADER_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_indexed_unchanged_headerfile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, HEADER_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_indexed_changed_sourcefile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, SOURCE_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_indexed_changed_sourcefile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, SOURCE_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());
	}

	void test_indexed_changed_headerfile_that_is_nottoindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, HEADER_FILE, NOT_TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
	}

	void test_indexed_changed_headerfile_that_is_toindex()
	{
		const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, HEADER_FILE, TO_INDEX);
		TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
		TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
		TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
		TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size()); // the header file will only be indexed on demand
	}

	// Now we test some referencing stuff

	void test_refresh_info_for_updated_files_clears_and_reindexes_known_outdated_file_and_referencing_source_file()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath outdatedSourceFilePath = m_sourceFolder.getConcatenated(L"outdated_file.cpp");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({
				upToDateSourceFilePath,
				outdatedSourceFilePath
			})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id upToDateSourceFileId = addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);
			const Id outdatedSourceFileId = addVeryOldFileToStorage(outdatedSourceFilePath, true, true, storage);
			addFileToFileSystem(outdatedSourceFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outdatedSourceFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(2, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(2, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), outdatedSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), outdatedSourceFilePath
			));
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_clears_known_outdated_header_file_and_reindexes_referencing_source_file()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath outdatedHeaderFilePath = m_sourceFolder.getConcatenated(L"outdated_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(
				{
					upToDateSourceFilePath
				},
				{
					upToDateSourceFilePath,
					outdatedHeaderFilePath
				}
			)));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id upToDateSourceFileId = addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);
			const Id outdatedHeaderFileId = addVeryOldFileToStorage(outdatedHeaderFilePath, true, true, storage);
			addFileToFileSystem(outdatedHeaderFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outdatedHeaderFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(2, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), outdatedHeaderFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath
			));
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_clears_unknown_outdated_header_file_and_reindexes_referencing_source()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath outdatedHeaderFilePath = m_sourceFolder.getConcatenated(L"outdated_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({
				upToDateSourceFilePath
			})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id upToDateSourceFileId = addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);
			const Id outdatedHeaderFileId = addVeryOldFileToStorage(outdatedHeaderFilePath, false, true, storage);
			addFileToFileSystem(outdatedHeaderFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outdatedHeaderFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.nonIndexedFilesToClear), outdatedHeaderFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath
			));
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_does_not_clear_unknown_uptodate_header_file()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(
				{
					upToDateSourceFilePath
				}
			)));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id upToDateSourceFileId = addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);
			const Id upToDateHeaderFileId = addVeryNewFileToStorage(upToDateHeaderFilePath, false, true, storage);
			addFileToFileSystem(upToDateHeaderFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_clears_outdated_source_file_and_referened_uptodate_header_file()
	{
		cleanup();
		{
			const FilePath outdatedSourceFilePath = m_sourceFolder.getConcatenated(L"outdated_file.cpp");
			const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({
				outdatedSourceFilePath
			},
			{
				outdatedSourceFilePath,
				upToDateHeaderFilePath
			})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id outdatedSourceFileId = addVeryOldFileToStorage(outdatedSourceFilePath, true, true, storage);
			addFileToFileSystem(outdatedSourceFilePath);
			const Id upToDateHeaderFileId = addVeryNewFileToStorage(upToDateHeaderFilePath, true, true, storage);
			addFileToFileSystem(upToDateHeaderFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, outdatedSourceFileId, upToDateHeaderFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(2, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), outdatedSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), upToDateHeaderFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), outdatedSourceFilePath
			));
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_does_not_clear_uptodate_header_referenced_by_uptodate_source()
	{
		cleanup();
		{
			const FilePath outdatedSourceFilePath = m_sourceFolder.getConcatenated(L"outdated_file.cpp");
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({
				outdatedSourceFilePath,
				upToDateSourceFilePath
			},
			{
				outdatedSourceFilePath,
				upToDateSourceFilePath,
				upToDateHeaderFilePath
			})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id outdatedSourceFileId = addVeryOldFileToStorage(outdatedSourceFilePath, true, true, storage);
			addFileToFileSystem(outdatedSourceFilePath);
			const Id upToDateSourceFileId = addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);
			const Id upToDateHeaderFileId = addVeryNewFileToStorage(upToDateHeaderFilePath, true, true, storage);
			addFileToFileSystem(upToDateHeaderFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, outdatedSourceFileId, upToDateHeaderFileId));
			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), outdatedSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), outdatedSourceFilePath
			));
		}
		cleanup();
	}

	void test_clears_unchanged_files_referenced_by_unchanged_file_that_referenced_changed_indexed_file()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.h");
			const FilePath outOfDateHeaderFilePath = m_sourceFolder.getConcatenated(L"out_of_date_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({
				upToDateSourceFilePath
			},
			{
				upToDateSourceFilePath,
				outOfDateHeaderFilePath,
			})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id upToDateSourceFileId = addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);
			const Id upToDateHeaderFileId = addVeryNewFileToStorage(upToDateHeaderFilePath, false, true, storage);
			addFileToFileSystem(upToDateHeaderFilePath);
			const Id outOfDateHeaderFileId = addVeryOldFileToStorage(outOfDateHeaderFilePath, true, true, storage);
			addFileToFileSystem(outOfDateHeaderFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));
			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outOfDateHeaderFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(2, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.nonIndexedFilesToClear), upToDateHeaderFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), outOfDateHeaderFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath
			));
		}
		cleanup();
	}

	void test_clears_unchanged_files_referenced_by_unchanged_file_that_referenced_changed_nonindexed_file()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.h");
			const FilePath outOfDateHeaderFilePath = m_sourceFolder.getConcatenated(L"out_of_date_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({
				upToDateSourceFilePath
			},
			{
				upToDateSourceFilePath,
				upToDateHeaderFilePath,
			})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			const Id upToDateSourceFileId = addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);
			const Id upToDateHeaderFileId = addVeryNewFileToStorage(upToDateHeaderFilePath, true, true, storage);
			addFileToFileSystem(upToDateHeaderFilePath);
			const Id outOfDateHeaderFileId = addVeryOldFileToStorage(outOfDateHeaderFilePath, false, true, storage);
			addFileToFileSystem(outOfDateHeaderFilePath);

			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));
			storage->addEdge(StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outOfDateHeaderFileId));

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(1, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(2, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.nonIndexedFilesToClear), outOfDateHeaderFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), upToDateHeaderFilePath
			));
			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath
			));
		}
		cleanup();
	}

private:
	class SourceGroupSettingsTest : public SourceGroupSettings
	{
	public:
		SourceGroupSettingsTest(const ProjectSettings* projectSettings)
			: SourceGroupSettings("TEST_ID", SOURCE_GROUP_UNKNOWN, projectSettings)
		{
		}

		std::shared_ptr<SourceGroupSettings> createCopy() const override
		{
			return std::shared_ptr<SourceGroupSettings>();
		}
	};

	class SourceGroupTest : public SourceGroup
	{
	public:

		SourceGroupTest(std::set<FilePath> sourceFilePaths)
			: m_sourceFilePaths(sourceFilePaths)
			, m_allFilePaths(sourceFilePaths)
		{
			m_sourceGroupSettings = std::make_shared<SourceGroupSettingsTest>(&m_projectSettings);
		}

		SourceGroupTest(std::set<FilePath> sourceFilePaths, std::set<FilePath> allFilePaths)
			: m_sourceFilePaths(sourceFilePaths)
			, m_allFilePaths(allFilePaths)
		{
			m_sourceGroupSettings = std::make_shared<SourceGroupSettingsTest>(&m_projectSettings);
		}

		std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override
		{
			std::set<FilePath> containedFilePaths;

			for (const FilePath& filePath : filePaths)
			{
				if (m_allFilePaths.find(filePath) != m_allFilePaths.end())
				{
					containedFilePaths.insert(filePath);
				}
			}

			return containedFilePaths;
		}

		std::set<FilePath> getAllSourceFilePaths() const override
		{
			return m_sourceFilePaths;
		}

		std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const std::set<FilePath>& filesToIndex) const override
		{
			return std::vector<std::shared_ptr<IndexerCommand>>();
		}

	private:
		std::shared_ptr<SourceGroupSettings> getSourceGroupSettings() override
		{
			return m_sourceGroupSettings;
		}

		std::shared_ptr<const SourceGroupSettings> getSourceGroupSettings() const override
		{
			return m_sourceGroupSettings;
		}

		ProjectSettings m_projectSettings;
		std::shared_ptr<SourceGroupSettingsTest> m_sourceGroupSettings;
		const std::set<FilePath> m_sourceFilePaths;
		const std::set<FilePath> m_allFilePaths;
	};

	void cleanup()
	{
		FileSystem::remove(m_indexDbPath);
		FileSystem::remove(m_bookmarkDbPath);

		if (m_sourceFolder.recheckExists())
		{
			for (const FilePath& path : FileSystem::getFilePathsFromDirectory(m_sourceFolder))
			{
				FileSystem::remove(path);
			}
			FileSystem::remove(m_sourceFolder);
		}
	}

	void addFileToFileSystem(const FilePath& filePath)
	{
		FileSystem::createDirectory(filePath.getParentDirectory());
		std::ofstream file;
		file.open(filePath.str());
		file << "This is some file content.\n";
		file.close();
	}

	Id addFileToStorage(const FilePath& filePath, const std::string& modificationTime, bool indexed, bool complete, std::shared_ptr<PersistentStorage> storage)
	{
		const Id id = storage->addNode(StorageNodeData(NodeType::NODE_FILE, NameHierarchy::serialize(NameHierarchy(filePath.wstr(), NAME_DELIMITER_FILE)))).first;
		storage->addFile(StorageFile(id, filePath.wstr(), modificationTime, indexed, complete));
		return id;
	}

	Id addVeryOldFileToStorage(const FilePath& filePath, bool indexed, bool complete, std::shared_ptr<PersistentStorage> storage)
	{
		return addFileToStorage(filePath, "2000-01-01 10:10:10", indexed, complete, storage);
	}

	Id addVeryNewFileToStorage(const FilePath& filePath, bool indexed, bool complete, std::shared_ptr<PersistentStorage> storage)
	{
		return addFileToStorage(filePath, "2020-01-01 10:10:10", indexed, complete, storage);
	}

};
