#include "cxxtest/TestSuite.h"

#include "data/storage/PersistentStorage.h"
#include "project/RefreshInfo.h"
#include "project/RefreshInfoGenerator.h"
#include "project/SourceGroup.h"
#include "settings/ProjectSettings.h"
#include "settings/SourceGroupSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/utility.h"

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

	void test_refresh_info_for_all_files_has_nothing_to_clear_and_specified_source_files_for_basic_sourcegroup()
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

	void test_refresh_info_for_updated_files_indexes_previously_unknown_file()
	{
		cleanup();
		{
			const FilePath unknownSourceFilePath = m_sourceFolder.getConcatenated(L"unknown_file.cpp");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({ unknownSourceFilePath })));

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
			TS_ASSERT_EQUALS(1, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToIndex), unknownSourceFilePath
			));
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_clears_non_existing_file()
	{
		cleanup();
		{
			const FilePath nonexistingSourceFilePath = m_sourceFolder.getConcatenated(L"non_existing_file.cpp");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({})));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			addVeryOldFileToStorage(nonexistingSourceFilePath, true, true, storage);

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(1, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());

			TS_ASSERT(utility::containsElement<FilePath>(
				utility::toVector(refreshInfo.filesToClear), nonexistingSourceFilePath
			));
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_clears_and_indexes_outdated_file()
	{
		cleanup();
		{
			const FilePath outdatedSourceFilePath = m_sourceFolder.getConcatenated(L"outdated_file.cpp");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({ outdatedSourceFilePath })));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			addVeryOldFileToStorage(outdatedSourceFilePath, true, true, storage);
			addFileToFileSystem(outdatedSourceFilePath);

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

	void test_refresh_info_for_updated_files_keeps_up_to_date_file()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({ upToDateSourceFilePath })));

			std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
				m_indexDbPath,
				m_bookmarkDbPath
			);
			storage->setup();

			addVeryNewFileToStorage(upToDateSourceFilePath, true, true, storage);
			addFileToFileSystem(upToDateSourceFilePath);

			storage->buildCaches();

			const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(sourceGroups, storage);

			TS_ASSERT_EQUALS(REFRESH_UPDATED_FILES, refreshInfo.mode);
			TS_ASSERT_EQUALS(0, refreshInfo.nonIndexedFilesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToClear.size());
			TS_ASSERT_EQUALS(0, refreshInfo.filesToIndex.size());
		}
		cleanup();
	}

	void test_refresh_info_for_updated_files_clears_and_reindexes_outdated_file_and_referencing_source_file()
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

	void test_refresh_info_for_updated_files_clears_outdated_header_file_and_reindexes_referencing_source()
	{
		cleanup();
		{
			const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(L"up_to_date_file.cpp");
			const FilePath outdatedHeaderFilePath = m_sourceFolder.getConcatenated(L"outdated_file.h");

			std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({
				upToDateSourceFilePath
			},
			{
				upToDateSourceFilePath,
				outdatedHeaderFilePath
			})));

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
		const Id id = storage->addNode(StorageNodeData(NodeType::NODE_FILE, NameHierarchy::serialize(NameHierarchy(filePath.wstr(), NAME_DELIMITER_FILE))));
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
