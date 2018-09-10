#ifndef COMBINED_INDEXER_COMMAND_PROVIDER_H
#define COMBINED_INDEXER_COMMAND_PROVIDER_H

#include <vector>

#include "data/indexer/IndexerCommandProvider.h"

class CombinedIndexerCommandProvider : public IndexerCommandProvider
{
public:
	void addProvider(std::shared_ptr<IndexerCommandProvider> provider);
	std::vector<FilePath> getAllSourceFilePaths() const override;
	std::shared_ptr<IndexerCommand> consumeCommand() override;
	std::shared_ptr<IndexerCommand> consumeCommandForSourceFilePath(const FilePath& filePath) override;
	std::vector<std::shared_ptr<IndexerCommand>> consumeAllCommands() override;
	void clear() override;
	size_t size() const override;

private:
	std::vector<std::shared_ptr<IndexerCommandProvider>> m_providers;
};

#endif // COMBINED_INDEXER_COMMAND_PROVIDER_H
