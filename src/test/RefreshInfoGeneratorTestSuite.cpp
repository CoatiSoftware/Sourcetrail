#include "catch.hpp"

#include <QDateTime>

#include "FileSystem.h"
#include "PersistentStorage.h"
#include "ProjectSettings.h"
#include "RefreshInfo.h"
#include "RefreshInfoGenerator.h"
#include "SourceGroup.h"
#include "SourceGroupSettings.h"
#include "utility.h"

namespace
{
FilePath m_indexDbPath = FilePath(L"data/RefreshInfoGeneratorTestSuite/project.srctrldb");
FilePath m_bookmarkDbPath = FilePath(L"data/RefreshInfoGeneratorTestSuite/project.srctrlbm");
FilePath m_sourceFolder = FilePath(L"data/RefreshInfoGeneratorTestSuite/src");

class SourceGroupSettingsTest: public SourceGroupSettings
{
public:
	SourceGroupSettingsTest(const ProjectSettings* projectSettings)
		: SourceGroupSettings(SOURCE_GROUP_UNKNOWN, "TEST_ID", projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return nullptr;
	}

	void loadSettings(const ConfigManager* config) override {}

	void saveSettings(ConfigManager* config) override {}

	bool equalsSettings(const SourceGroupSettingsBase* other) override
	{
		return true;
	}
};

class SourceGroupTest: public SourceGroup
{
public:
	SourceGroupTest(std::set<FilePath> sourceFilePaths)
		: m_sourceFilePaths(sourceFilePaths), m_allFilePaths(sourceFilePaths)
	{
		m_sourceGroupSettings = std::make_shared<SourceGroupSettingsTest>(&m_projectSettings);
	}

	SourceGroupTest(std::set<FilePath> sourceFilePaths, std::set<FilePath> allFilePaths)
		: m_sourceFilePaths(sourceFilePaths), m_allFilePaths(allFilePaths)
	{
		m_sourceGroupSettings = std::make_shared<SourceGroupSettingsTest>(&m_projectSettings);
	}

	std::set<FilePath> filterToContainedFilePaths(const std::set<FilePath>& filePaths) const override
	{
		std::set<FilePath> containedFilePaths;

		for (const FilePath& filePath: filePaths)
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

	std::vector<std::shared_ptr<IndexerCommand>> getIndexerCommands(const RefreshInfo& info) const override
	{
		return std::vector<std::shared_ptr<IndexerCommand>>();
	}

	void setStatus(SourceGroupStatusType status)
	{
		m_sourceGroupSettings->setStatus(status);
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
		for (const FilePath& path: FileSystem::getFilePathsFromDirectory(m_sourceFolder))
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

Id addFileToStorage(
	const FilePath& filePath,
	const std::string& modificationTime,
	bool indexed,
	bool complete,
	std::shared_ptr<PersistentStorage> storage)
{
	const Id id =
		storage
			->addNode(StorageNodeData(
				NODE_FILE,
				NameHierarchy::serialize(NameHierarchy(filePath.wstr(), NAME_DELIMITER_FILE))))
			.first;
	storage->addFile(
		StorageFile(id, filePath.wstr(), L"someLanguage", modificationTime, indexed, complete));
	return id;
}

Id addVeryOldFileToStorage(
	const FilePath& filePath, bool indexed, bool complete, std::shared_ptr<PersistentStorage> storage)
{
	return addFileToStorage(
		filePath,
		QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd hh:mm:ss").toStdString(),
		indexed,
		complete,
		storage);
}

Id addVeryNewFileToStorage(
	const FilePath& filePath, bool indexed, bool complete, std::shared_ptr<PersistentStorage> storage)
{
	return addFileToStorage(
		filePath,
		QDateTime::currentDateTime().addDays(1).toString("yyyy-MM-dd hh:mm:ss").toStdString(),
		indexed,
		complete,
		storage);
}
}	 // namespace

TEST_CASE("refresh info for all files is empty for empty project")
{
	std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
	sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({})));

	const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForAllFiles(sourceGroups);

	REQUIRE(REFRESH_ALL_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("refresh info for all files clears nothing and indexes previously unknown source file")
{
	cleanup();
	{
		const FilePath sourceFilePath = m_sourceFolder.getConcatenated(L"main.cpp");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(
			std::shared_ptr<SourceGroupTest>(new SourceGroupTest({sourceFilePath})));

		addFileToFileSystem(sourceFilePath);

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForAllFiles(sourceGroups);

		REQUIRE(REFRESH_ALL_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(0 == refreshInfo.filesToClear.size());
		REQUIRE(1 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), sourceFilePath));
	}
	cleanup();
}

TEST_CASE("refresh info for all files is empty for disabled source group")
{
	cleanup();
	{
		const FilePath sourceFilePath = m_sourceFolder.getConcatenated(L"main.cpp");

		std::shared_ptr<SourceGroupTest> sourceGroup(new SourceGroupTest({sourceFilePath}));
		sourceGroup->setStatus(SOURCE_GROUP_STATUS_DISABLED);

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(sourceGroup);

		addFileToFileSystem(sourceFilePath);

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForAllFiles(sourceGroups);

		REQUIRE(REFRESH_ALL_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(0 == refreshInfo.filesToClear.size());
		REQUIRE(0 == refreshInfo.filesToIndex.size());
	}
	cleanup();
}

TEST_CASE("refresh info for all files is clears indexed files of disabled source group")
{
	cleanup();
	{
		const FilePath sourceFilePath = m_sourceFolder.getConcatenated(L"main.cpp");

		std::shared_ptr<SourceGroupTest> sourceGroup(new SourceGroupTest({sourceFilePath}));
		sourceGroup->setStatus(SOURCE_GROUP_STATUS_DISABLED);

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(sourceGroup);

		addFileToFileSystem(sourceFilePath);

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();
		addVeryNewFileToStorage(sourceFilePath, true, true, storage);
		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(1 == refreshInfo.filesToClear.size());
		REQUIRE(0 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), sourceFilePath));
	}
	cleanup();
}

TEST_CASE("refresh info for all files is clears nonindexed files of disabled source group")
{
	cleanup();
	{
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.h");

		std::shared_ptr<SourceGroupTest> sourceGroup(new SourceGroupTest(
			{upToDateSourceFilePath}, {upToDateSourceFilePath, upToDateHeaderFilePath}));
		sourceGroup->setStatus(SOURCE_GROUP_STATUS_DISABLED);

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(sourceGroup);

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id upToDateHeaderFileId = addVeryNewFileToStorage(
			upToDateHeaderFilePath, false, true, storage);
		addFileToFileSystem(upToDateHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(1 == refreshInfo.filesToClear.size());
		REQUIRE(0 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath));

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.nonIndexedFilesToClear), upToDateHeaderFilePath));
	}
	cleanup();
}

TEST_CASE("refresh info for updated files is empty for empty storage and empty sourcegroup")
{
	cleanup();
	{
		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(0 == refreshInfo.filesToClear.size());
		REQUIRE(0 == refreshInfo.filesToIndex.size());
	}
	cleanup();
}

// Now we will test how the refresh info generator reacts to different situations when generating
// refresh info for updated files. A file can have different states in the following dimensions:
//	file may be known by the storage					unknown / nonindexed / indexed
//	file may be changed									unchanged / changed
//	file may be a source file							sourcefile / headerfile
//	file may now be indexed by the source group			nottoindex / toindex
//
// We now wite a TEST_CASE that checks for the expected result for every possible combination.
//	Example: test_unknown_unchanged_sourcefile_that_is_nottoindex
namespace
{
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

RefreshInfo getRefreshInfo(
	KnownState knownState, ChangedState changedState, FileState fileState, IndexingState indexingState)
{
	RefreshInfo refreshInfo;
	cleanup();
	{
		const FilePath filePath = m_sourceFolder.getConcatenated(L"file.extension");

		const std::set<FilePath> sourceFilePaths =
			((fileState == SOURCE_FILE) ? std::set<FilePath>({filePath}) : std::set<FilePath>({}));
		const std::set<FilePath> allFilePaths = {filePath};

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		if (indexingState == NOT_TO_INDEX)
		{
			// a file is "not existing" if the source group does not care about it
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest({}, {})));
		}
		else	// if (indexingState == TO_INDEX)
		{
			sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(
				new SourceGroupTest(sourceFilePaths, allFilePaths)));
		}

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
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
			else	// if (changedState == CHANGED)
			{
				addVeryOldFileToStorage(filePath, false, true, storage);
			}
		}
		else	// if (knownState == INDEXED)
		{
			if (changedState == UNCHANGED)
			{
				addVeryNewFileToStorage(filePath, true, true, storage);
			}
			else	// if (changedState == CHANGED)
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
}	 // namespace

TEST_CASE("unknown unchanged sourcefile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, SOURCE_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("unknown unchanged sourcefile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, SOURCE_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(1 == refreshInfo.filesToIndex.size());
}

TEST_CASE("unknown unchanged headerfile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, HEADER_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("unknown unchanged headerfile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, UNCHANGED, HEADER_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
	;	 // the header file will only be indexed on demand
}

TEST_CASE("unknown changed sourcefile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, SOURCE_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("unknown changed sourcefile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, SOURCE_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(1 == refreshInfo.filesToIndex.size());
}

TEST_CASE("unknown changed headerfile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, HEADER_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("unknown changed headerfile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(UNKNOWN, CHANGED, HEADER_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
	;	 // the header file will only be indexed on demand
}

TEST_CASE("nonindexed unchanged sourcefile that is nottoindex")	   // this test does not really make sense
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, SOURCE_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("nonindexed unchanged sourcefile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, SOURCE_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(1 == refreshInfo.filesToIndex.size());
}

TEST_CASE("nonindexed unchanged headerfile that is nottoindex")	   // this test does not make much
																   // sense without source files
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, HEADER_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("nonindexed unchanged headerfile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, UNCHANGED, HEADER_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
	;	 // the header file will only be indexed on demand
}

TEST_CASE("nonindexed changed sourcefile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, SOURCE_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("nonindexed changed sourcefile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, SOURCE_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(1 == refreshInfo.filesToIndex.size());
}

TEST_CASE("nonindexed changed headerfile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, HEADER_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
	;	 // must be cleard here and will be re-indexed on demand
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("nonindexed changed headerfile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(NON_INDEXED, CHANGED, HEADER_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());	  // the header file will only be indexed on demand
}

TEST_CASE("indexed unchanged sourcefile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, SOURCE_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(1 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("indexed unchanged sourcefile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, SOURCE_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("indexed unchanged headerfile that is nottoindex")	// TODO: check if depending source
																// file gets reindexed
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, HEADER_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(1 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("indexed unchanged headerfile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, UNCHANGED, HEADER_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(0 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("indexed changed sourcefile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, SOURCE_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(1 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("indexed changed sourcefile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, SOURCE_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(1 == refreshInfo.filesToClear.size());
	REQUIRE(1 == refreshInfo.filesToIndex.size());
}

TEST_CASE("indexed changed headerfile that is nottoindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, HEADER_FILE, NOT_TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(1 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());
}

TEST_CASE("indexed changed headerfile that is toindex")
{
	const RefreshInfo refreshInfo = getRefreshInfo(INDEXED, CHANGED, HEADER_FILE, TO_INDEX);
	REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
	REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
	REQUIRE(1 == refreshInfo.filesToClear.size());
	REQUIRE(0 == refreshInfo.filesToIndex.size());	  // the header file will only be indexed on demand
}

// Now we test some referencing stuff

TEST_CASE(
	"refresh info for updated files clears and reindexes known outdated file and referencing "
	"source file")
{
	cleanup();
	{
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath outdatedSourceFilePath = m_sourceFolder.getConcatenated(
			L"outdated_file.cpp");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(
			new SourceGroupTest({upToDateSourceFilePath, outdatedSourceFilePath})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id outdatedSourceFileId = addVeryOldFileToStorage(
			outdatedSourceFilePath, true, true, storage);
		addFileToFileSystem(outdatedSourceFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outdatedSourceFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(2 == refreshInfo.filesToClear.size());
		REQUIRE(2 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), outdatedSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), outdatedSourceFilePath));
	}
	cleanup();
}

TEST_CASE(
	"refresh info for updated files clears known outdated header file and reindexes referencing "
	"source file")
{
	cleanup();
	{
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath outdatedHeaderFilePath = m_sourceFolder.getConcatenated(L"outdated_file.h");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(
			{upToDateSourceFilePath}, {upToDateSourceFilePath, outdatedHeaderFilePath})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id outdatedHeaderFileId = addVeryOldFileToStorage(
			outdatedHeaderFilePath, true, true, storage);
		addFileToFileSystem(outdatedHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outdatedHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(2 == refreshInfo.filesToClear.size());
		REQUIRE(1 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), outdatedHeaderFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath));
	}
	cleanup();
}

TEST_CASE(
	"refresh info for updated files clears unknown outdated header file and reindexes referencing "
	"source")
{
	cleanup();
	{
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath outdatedHeaderFilePath = m_sourceFolder.getConcatenated(L"outdated_file.h");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(
			std::shared_ptr<SourceGroupTest>(new SourceGroupTest({upToDateSourceFilePath})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id outdatedHeaderFileId = addVeryOldFileToStorage(
			outdatedHeaderFilePath, false, true, storage);
		addFileToFileSystem(outdatedHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outdatedHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(1 == refreshInfo.filesToClear.size());
		REQUIRE(1 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.nonIndexedFilesToClear), outdatedHeaderFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath));
	}
	cleanup();
}

TEST_CASE("refresh info for updated files does not clear unknown uptodate header file")
{
	cleanup();
	{
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.h");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(
			std::shared_ptr<SourceGroupTest>(new SourceGroupTest({upToDateSourceFilePath})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id upToDateHeaderFileId = addVeryNewFileToStorage(
			upToDateHeaderFilePath, false, true, storage);
		addFileToFileSystem(upToDateHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(0 == refreshInfo.filesToClear.size());
		REQUIRE(0 == refreshInfo.filesToIndex.size());
	}
	cleanup();
}

TEST_CASE(
	"refresh info for updated files clears outdated source file and referened uptodate header file")
{
	cleanup();
	{
		const FilePath outdatedSourceFilePath = m_sourceFolder.getConcatenated(
			L"outdated_file.cpp");
		const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.h");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(
			{outdatedSourceFilePath}, {outdatedSourceFilePath, upToDateHeaderFilePath})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id outdatedSourceFileId = addVeryOldFileToStorage(
			outdatedSourceFilePath, true, true, storage);
		addFileToFileSystem(outdatedSourceFilePath);
		const Id upToDateHeaderFileId = addVeryNewFileToStorage(
			upToDateHeaderFilePath, true, true, storage);
		addFileToFileSystem(upToDateHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, outdatedSourceFileId, upToDateHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(2 == refreshInfo.filesToClear.size());
		REQUIRE(1 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), outdatedSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateHeaderFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), outdatedSourceFilePath));
	}
	cleanup();
}

TEST_CASE(
	"refresh info for updated files does not clear uptodate header referenced by uptodate source")
{
	cleanup();
	{
		const FilePath outdatedSourceFilePath = m_sourceFolder.getConcatenated(
			L"outdated_file.cpp");
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.h");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(
			{outdatedSourceFilePath, upToDateSourceFilePath},
			{outdatedSourceFilePath, upToDateSourceFilePath, upToDateHeaderFilePath})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id outdatedSourceFileId = addVeryOldFileToStorage(
			outdatedSourceFilePath, true, true, storage);
		addFileToFileSystem(outdatedSourceFilePath);
		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id upToDateHeaderFileId = addVeryNewFileToStorage(
			upToDateHeaderFilePath, true, true, storage);
		addFileToFileSystem(upToDateHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, outdatedSourceFileId, upToDateHeaderFileId));
		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(0 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(1 == refreshInfo.filesToClear.size());
		REQUIRE(1 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), outdatedSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), outdatedSourceFilePath));
	}
	cleanup();
}

TEST_CASE(
	"clears unchanged files referenced by unchanged file that referenced changed indexed file")
{
	cleanup();
	{
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.h");
		const FilePath outOfDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"out_of_date_file.h");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(
			{upToDateSourceFilePath},
			{
				upToDateSourceFilePath,
				outOfDateHeaderFilePath,
			})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id upToDateHeaderFileId = addVeryNewFileToStorage(
			upToDateHeaderFilePath, false, true, storage);
		addFileToFileSystem(upToDateHeaderFilePath);
		const Id outOfDateHeaderFileId = addVeryOldFileToStorage(
			outOfDateHeaderFilePath, true, true, storage);
		addFileToFileSystem(outOfDateHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));
		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outOfDateHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(2 == refreshInfo.filesToClear.size());
		REQUIRE(1 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.nonIndexedFilesToClear), upToDateHeaderFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), outOfDateHeaderFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath));
	}
	cleanup();
}

TEST_CASE(
	"clears unchanged files referenced by unchanged file that referenced changed nonindexed file")
{
	cleanup();
	{
		const FilePath upToDateSourceFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.cpp");
		const FilePath upToDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"up_to_date_file.h");
		const FilePath outOfDateHeaderFilePath = m_sourceFolder.getConcatenated(
			L"out_of_date_file.h");

		std::vector<std::shared_ptr<SourceGroup>> sourceGroups;
		sourceGroups.push_back(std::shared_ptr<SourceGroupTest>(new SourceGroupTest(
			{upToDateSourceFilePath},
			{
				upToDateSourceFilePath,
				upToDateHeaderFilePath,
			})));

		std::shared_ptr<PersistentStorage> storage = std::make_shared<PersistentStorage>(
			m_indexDbPath, m_bookmarkDbPath);
		storage->setup();

		const Id upToDateSourceFileId = addVeryNewFileToStorage(
			upToDateSourceFilePath, true, true, storage);
		addFileToFileSystem(upToDateSourceFilePath);
		const Id upToDateHeaderFileId = addVeryNewFileToStorage(
			upToDateHeaderFilePath, true, true, storage);
		addFileToFileSystem(upToDateHeaderFilePath);
		const Id outOfDateHeaderFileId = addVeryOldFileToStorage(
			outOfDateHeaderFilePath, false, true, storage);
		addFileToFileSystem(outOfDateHeaderFilePath);

		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, upToDateHeaderFileId));
		storage->addEdge(
			StorageEdgeData(Edge::EDGE_INCLUDE, upToDateSourceFileId, outOfDateHeaderFileId));

		storage->buildCaches();

		const RefreshInfo refreshInfo = RefreshInfoGenerator::getRefreshInfoForUpdatedFiles(
			sourceGroups, storage);

		REQUIRE(REFRESH_UPDATED_FILES == refreshInfo.mode);
		REQUIRE(1 == refreshInfo.nonIndexedFilesToClear.size());
		REQUIRE(2 == refreshInfo.filesToClear.size());
		REQUIRE(1 == refreshInfo.filesToIndex.size());

		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.nonIndexedFilesToClear), outOfDateHeaderFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateSourceFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToClear), upToDateHeaderFilePath));
		REQUIRE(utility::containsElement<FilePath>(
			utility::toVector(refreshInfo.filesToIndex), upToDateSourceFilePath));
	}
	cleanup();
}
