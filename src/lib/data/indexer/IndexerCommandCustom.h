#ifndef INDEXER_COMMAND_CUSTOM_H
#define INDEXER_COMMAND_CUSTOM_H

#include <vector>
#include <string>

#include "IndexerCommand.h"

class IndexerCommandCustom
	: public IndexerCommand
{
public:
	static IndexerCommandType getStaticIndexerCommandType();

	IndexerCommandCustom(const FilePath& sourceFilePath, const std::wstring& customCommand);

	IndexerCommandType getIndexerCommandType() const override;
	size_t getByteSize(size_t stringSize) const override;

	const std::wstring& getCustomCommand() const;

protected:
	QJsonObject doSerialize() const override;

private:
	std::wstring m_customCommand;
};

#endif // INDEXER_COMMAND_CXXL_H
