#include "TaskExecuteCustomCommands.h"

#include "Blackboard.h"
#include "DialogView.h"
#include "IndexerCommandCustom.h"
#include "IndexerCommandProvider.h"
#include "MessageIndexingStatus.h"
#include "MessageShowStatus.h"
#include "MessageStatus.h"
#include "PersistentStorage.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityString.h"

TaskExecuteCustomCommands::TaskExecuteCustomCommands(
	std::unique_ptr<IndexerCommandProvider> indexerCommandProvider,
	std::shared_ptr<PersistentStorage> storage,
	std::shared_ptr<DialogView> dialogView,
	const FilePath& projectDirectory
)
	: m_indexerCommandProvider(std::move(indexerCommandProvider))
	, m_storage(storage)
	, m_dialogView(dialogView)
	, m_projectDirectory(projectDirectory)
{
}

void TaskExecuteCustomCommands::doEnter(std::shared_ptr<Blackboard> blackboard)
{
	m_dialogView->hideUnknownProgressDialog();
	m_start = utility::durationStart();
}

Task::TaskState TaskExecuteCustomCommands::doUpdate(std::shared_ptr<Blackboard> blackboard)
{
	if (m_interrupted)
	{
		return STATE_SUCCESS;
	}

	int sourceFileCount = m_indexerCommandProvider->size();
	int indexedSourceFileCount = 0;
	m_dialogView->updateCustomIndexingDialog(0, 0, sourceFileCount, { });

	while (!m_interrupted && m_indexerCommandProvider->size())
	{
		std::shared_ptr<IndexerCommandCustom> indexerCommand =
			std::dynamic_pointer_cast<IndexerCommandCustom>(m_indexerCommandProvider->consumeCommand());
		if (indexerCommand)
		{
			FilePath sourcePath = indexerCommand->getSourceFilePath();
			m_dialogView->updateCustomIndexingDialog(indexedSourceFileCount + 1, indexedSourceFileCount, sourceFileCount, { sourcePath });
			MessageIndexingStatus(true, indexedSourceFileCount * 100 / sourceFileCount).dispatch();
			LOG_INFO_STREAM(<< "Execute command \"" << utility::encodeToUtf8(indexerCommand->getCustomCommand()) << "\"");

			m_storage->beforeErrorRecording();

			std::wstring processOutput;
			int result = utility::executeProcessAndGetExitCode(indexerCommand->getCustomCommand(), {}, m_projectDirectory, -1, &processOutput);

			m_storage->afterErrorRecording();

			if (processOutput.size() > 3 || result != 0)
			{
				if (result == 0)
				{
					LOG_INFO_STREAM(<< "process return 0:\n" << utility::encodeToUtf8(processOutput));
				}
				else
				{
					LOG_ERROR_STREAM(<< "process returned " << result << ":\n" << utility::encodeToUtf8(processOutput));
					MessageShowStatus().dispatch();
					MessageStatus(L"command <" + indexerCommand->getCustomCommand() + L"> returned " +
						std::to_wstring(result) + L": " + processOutput, true, false, true).dispatch();
				}
			}

			indexedSourceFileCount++;
			blackboard->update<int>("indexed_source_file_count", [=](int count) { return count + 1; });
		}
	}

	return STATE_SUCCESS;
}

void TaskExecuteCustomCommands::doExit(std::shared_ptr<Blackboard> blackboard)
{
	float duration = utility::duration(m_start);
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
