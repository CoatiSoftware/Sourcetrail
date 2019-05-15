#include "TaskExecuteCustomCommands.h"

#include "ApplicationSettings.h"
#include "Blackboard.h"
#include "DialogView.h"
#include "ElementComponentKind.h"
#include "FileSystem.h"
#include "IndexerCommandCustom.h"
#include "IndexerCommandProvider.h"
#include "MessageIndexingStatus.h"
#include "MessageShowStatus.h"
#include "MessageStatus.h"
#include "PersistentStorage.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"
#include "TextAccess.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityString.h"

TaskExecuteCustomCommands::TaskExecuteCustomCommands(
	std::unique_ptr<IndexerCommandProvider> indexerCommandProvider,
	std::shared_ptr<PersistentStorage> storage,
	std::shared_ptr<DialogView> dialogView,
	size_t indexerThreadCount,
	const FilePath& projectDirectory
)
	: m_indexerCommandProvider(std::move(indexerCommandProvider))
	, m_storage(storage)
	, m_dialogView(dialogView)
	, m_indexerThreadCount(indexerThreadCount)
	, m_projectDirectory(projectDirectory)
	, m_indexerCommandCount(m_indexerCommandProvider->size())
	, m_hasPythonCommands(false)
{
}

void TaskExecuteCustomCommands::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_dialogView->hideUnknownProgressDialog();
	m_start = utility::durationStart();

	if (m_indexerCommandProvider)
	{
		while (!m_indexerCommandProvider->empty())
		{
			if (std::shared_ptr<IndexerCommandCustom> indexerCommand =
				std::dynamic_pointer_cast<IndexerCommandCustom>(m_indexerCommandProvider->consumeCommand()))
			{
				if (m_targetDatabaseFilePath.empty())
				{
					m_targetDatabaseFilePath = indexerCommand->getDatabaseFilePath();
				}

				if (indexerCommand->getIndexerCommandType() == INDEXER_COMMAND_PYTHON)
				{
					m_hasPythonCommands = true;
				}

				if (indexerCommand->getRunInParallel())
				{
					m_parallelCommands.push_back(indexerCommand);
				}
				else
				{
					m_serialCommands.push_back(indexerCommand);
				}
			}
		}
	}
}

Task::TaskState TaskExecuteCustomCommands::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_interrupted)
	{
		return STATE_SUCCESS;
	}

	m_dialogView->updateCustomIndexingDialog(0, 0, m_indexerCommandProvider->size(), {});


	std::vector<std::shared_ptr<std::thread>> indexerThreads;
	for (size_t i = 1 /*this method is counting as the first thread*/; i < m_indexerThreadCount; i++)
	{
		indexerThreads.push_back(std::make_shared<std::thread>(&TaskExecuteCustomCommands::executeParallelIndexerCommands, this, i, blackboard));
	}

	while (!m_interrupted && !m_serialCommands.empty())
	{
		std::shared_ptr<IndexerCommandCustom> indexerCommand = m_serialCommands.back();
		m_serialCommands.pop_back();
		runIndexerCommand(indexerCommand, blackboard);
	}

	executeParallelIndexerCommands(0, blackboard);

	for (std::shared_ptr<std::thread> indexerThread : indexerThreads)
	{
		indexerThread->join();
	}
	indexerThreads.clear();

	{
		PersistentStorage targetStorage(m_targetDatabaseFilePath, FilePath());
		targetStorage.setup();
		targetStorage.setMode(SqliteIndexStorage::STORAGE_MODE_WRITE);
		targetStorage.buildCaches();
		for (const FilePath& sourceDatabaseFilePath : m_sourceDatabaseFilePaths)
		{
			{
				PersistentStorage sourceStorage(sourceDatabaseFilePath, FilePath());
				sourceStorage.setMode(SqliteIndexStorage::STORAGE_MODE_READ);
				sourceStorage.buildCaches();
				targetStorage.inject(&sourceStorage);
			}
			FileSystem::remove(sourceDatabaseFilePath);
		}

		if (m_hasPythonCommands && ApplicationSettings::getInstance()->getPythonPostProcessingEnabled())
		{
			runPythonPostProcessing(targetStorage);
		}
	}

	return STATE_SUCCESS;
}

void TaskExecuteCustomCommands::doExit(std::shared_ptr<Blackboard> blackboard)
{
	m_storage.reset();
	const float duration = utility::duration(m_start);
	blackboard->update<float>("index_time", [duration](float currentDuration) { return currentDuration + duration; });
}

void TaskExecuteCustomCommands::doReset(std::shared_ptr<Blackboard> blackboard)
{
}

void TaskExecuteCustomCommands::handleMessage(MessageIndexingInterrupted* message)
{
	LOG_INFO("Interrupting custom command execution.");

	m_interrupted = true;

	m_dialogView->showUnknownProgressDialog(L"Interrupting Indexing", L"Waiting for running\ncommand to finish");
}

void TaskExecuteCustomCommands::executeParallelIndexerCommands(int threadId, std::shared_ptr<Blackboard> blackboard)
{
	while (!m_interrupted)
	{
		std::shared_ptr<IndexerCommandCustom> indexerCommand;
		{
			std::lock_guard<std::mutex> lock(m_parallelCommandsMutex);
			if (m_parallelCommands.empty())
			{
				return;
			}
			indexerCommand = m_parallelCommands.back();
			m_parallelCommands.pop_back();
		}

		if (threadId != 0)
		{
			FilePath databaseFilePath = indexerCommand->getDatabaseFilePath();
			databaseFilePath = databaseFilePath.getParentDirectory().concatenate(databaseFilePath.fileName() + L"_thread" + std::to_wstring(threadId));

			bool databaseFilePathKnown = true;
			{
				std::lock_guard<std::mutex> lock(m_sourceDatabaseFilePathsMutex);
				if (m_sourceDatabaseFilePaths.find(databaseFilePath) == m_sourceDatabaseFilePaths.end())
				{
					m_sourceDatabaseFilePaths.insert(databaseFilePath);
					databaseFilePathKnown = false;
				}
			}

			if (!databaseFilePathKnown)
			{
				if (databaseFilePath.exists())
				{
					LOG_WARNING(L"Temporary storage \"" + databaseFilePath.wstr() + L"\" already exists on file system. File will be removed to avoid conflicts.");
					FileSystem::remove(databaseFilePath);
				}
				PersistentStorage sourceStorage(databaseFilePath, FilePath());
				sourceStorage.setup();
				sourceStorage.setMode(SqliteIndexStorage::STORAGE_MODE_WRITE);
				sourceStorage.buildCaches();
			}

			indexerCommand->setDatabaseFilePath(databaseFilePath);
		}

		runIndexerCommand(indexerCommand, blackboard);
	}
}

void TaskExecuteCustomCommands::runIndexerCommand(std::shared_ptr<IndexerCommandCustom> indexerCommand, std::shared_ptr<Blackboard> blackboard)
{
	if (indexerCommand)
	{
		int indexedSourceFileCount = 0;
		blackboard->get("indexed_source_file_count", indexedSourceFileCount);

		const FilePath sourcePath = indexerCommand->getSourceFilePath();

		m_dialogView->updateCustomIndexingDialog(indexedSourceFileCount + 1, indexedSourceFileCount, m_indexerCommandCount, { sourcePath });
		MessageIndexingStatus(true, indexedSourceFileCount * 100 / m_indexerCommandCount).dispatch();

		const std::wstring command = indexerCommand->getCustomCommand();

		LOG_INFO_STREAM(<< "Execute command \"" << utility::encodeToUtf8(command) << "\"");

		m_storage->beforeErrorRecording();

		std::wstring processOutput;
		std::wstring errorMessage;
		const int result = utility::executeProcessAndGetExitCode(command, {}, m_projectDirectory, -1, &processOutput, &errorMessage);

		m_storage->afterErrorRecording();

		if (errorMessage.size() > 0 || processOutput.size() > 3 || result != 0)
		{
			if (result == 0 && errorMessage.empty())
			{
				std::wstring message = L"Process returned successfully";
				if (processOutput.empty())
				{
					message += L".";
				}
				else
				{
					message += L" with message \"" + processOutput + L"\".";
				}
				message += L"\n";
				LOG_INFO(message);
			}
			else
			{
				LOG_ERROR_STREAM(<< "process returned \"" << result << "\" with message:\n" << utility::encodeToUtf8(processOutput));
				MessageShowStatus().dispatch();
				MessageStatus(
					L"command \"" + indexerCommand->getCustomCommand() + L"\" returned code \"" + std::to_wstring(result) + L"\"" +
					L" with message \"" + errorMessage + L"\"" +
					L" and output \"" + processOutput + L"\".", true, false, true).dispatch();
			}
		}

		indexedSourceFileCount++;
		blackboard->update<int>("indexed_source_file_count", [=](int count) { return count + 1; });
	}
}

void TaskExecuteCustomCommands::runPythonPostProcessing(PersistentStorage& storage)
{
	LOG_INFO("Starting Python post processing.");

	std::vector<Id> unsolvedLocationIds;
	for (const StorageSourceLocation location : storage.getStorageSourceLocations())
	{
		if (intToLocationType(location.type) == LOCATION_UNSOLVED)
		{
			unsolvedLocationIds.push_back(location.id);
		}
	}

	std::shared_ptr<SourceLocationCollection> locationCollection = storage.getSourceLocationsForLocationIds(unsolvedLocationIds);

	std::map<std::wstring, std::vector<StorageNode>> nodeNameToStorageNodes;
	if (locationCollection->getSourceLocationCount() > 0)
	{
		for (const StorageNode& node : storage.getStorageNodes())
		{
			nodeNameToStorageNodes[NameHierarchy::deserialize(node.serializedName).back().getName()].push_back(node);
		}
	}

	struct DataToInsert
	{
		StorageEdgeData edgeData;
		Id sourceLocationId;
	};

	storage.setMode(SqliteIndexStorage::STORAGE_MODE_READ);

	std::vector<DataToInsert> dataToInsert;
	std::set<Id> elementsToDelete;
	locationCollection->forEachSourceLocationFile([&nodeNameToStorageNodes, &storage, &dataToInsert, &elementsToDelete](std::shared_ptr<SourceLocationFile> locationFile)
		{
			std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(locationFile->getFilePath());

			if (textAccess)
			{
				locationFile->forEachStartSourceLocation([textAccess, &nodeNameToStorageNodes, &storage, &dataToInsert, &elementsToDelete](const SourceLocation* startLoc)
					{
						if (!startLoc)
						{
							return;
						}
						const SourceLocation* endLoc = startLoc->getOtherLocation();
						if (!endLoc)
						{
							return;
						}

						const std::wstring token = utility::decodeFromUtf8(textAccess->getLine(startLoc->getLineNumber()).substr(startLoc->getColumnNumber() - 1, endLoc->getColumnNumber() - startLoc->getColumnNumber() + 1));

						for (const Id tokenId : startLoc->getTokenIds())
						{
							const StorageEdge edge = storage.getEdgeById(tokenId);
							if (edge.id != 0)
							{
								for (const StorageNode& targetNode : nodeNameToStorageNodes[token])
								{
									if (Edge::intToType(edge.type) == Edge::EDGE_CALL &&
										(
											NodeType::intToType(targetNode.type) != NodeType::NODE_FUNCTION ||
											NodeType::intToType(targetNode.type) != NodeType::NODE_METHOD
										)
									){
										continue;
									}
									dataToInsert.push_back({ StorageEdgeData(edge.type, edge.sourceNodeId, targetNode.id) , startLoc->getLocationId() });
									elementsToDelete.insert(edge.id);
								}
							}
						}
					}
				);
			}
		}
	);

	storage.setMode(SqliteIndexStorage::STORAGE_MODE_WRITE);

	storage.startInjection();

	std::vector<StorageEdge> edgesToInsert;
	for (const DataToInsert& data : dataToInsert)
	{
		edgesToInsert.push_back(StorageEdge(0, data.edgeData));
	}
	const std::vector<Id> ambiguousEdgeIds = storage.addEdges(edgesToInsert);

	if (ambiguousEdgeIds.size() == dataToInsert.size())
	{
		for (size_t i = 0; i < ambiguousEdgeIds.size(); i++)
		{
			storage.addElementComponent(StorageElementComponentData(ambiguousEdgeIds[i], elementComponentKindToInt(ElementComponentKind::IS_AMBIGUOUS), L""));
			storage.addOccurrence(StorageOccurrence(ambiguousEdgeIds[i], dataToInsert[i].sourceLocationId));
		}

		storage.removeElements(utility::toVector(elementsToDelete));
		storage.finishInjection();
	}
	else
	{
		LOG_ERROR("Error occurred while running Python post processing. Rolling back all changes.");
		storage.rollbackInjection();
	}

	LOG_INFO("Finished Python post processing.");
}
