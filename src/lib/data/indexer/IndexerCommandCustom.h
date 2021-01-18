#ifndef INDEXER_COMMAND_CUSTOM_H
#define INDEXER_COMMAND_CUSTOM_H

#include <string>
#include <vector>

#include "IndexerCommand.h"

class IndexerCommandCustom: public IndexerCommand
{
public:
	static IndexerCommandType getStaticIndexerCommandType();

	IndexerCommandCustom(
		const std::wstring& command,
		const std::vector<std::wstring>& arguments,
		const FilePath& projectFilePath,
		const FilePath& databaseFilePath,
		const std::wstring& databaseVersion,
		const FilePath& sourceFilePath,
		bool runInParallel);

	IndexerCommandCustom(
		IndexerCommandType type,
		const std::wstring& command,
		const std::vector<std::wstring>& arguments,
		const FilePath& projectFilePath,
		const FilePath& databaseFilePath,
		const std::wstring& databaseVersion,
		const FilePath& sourceFilePath,
		bool runInParallel);

	IndexerCommandType getIndexerCommandType() const override;
	size_t getByteSize(size_t stringSize) const override;

	FilePath getDatabaseFilePath() const;
	void setDatabaseFilePath(const FilePath& databaseFilePath);

	std::wstring getCommand() const;
	std::vector<std::wstring> getArguments() const;
	bool getRunInParallel() const;

protected:
	QJsonObject doSerialize() const override;

private:
	std::wstring replaceVariables(std::wstring s) const;

	IndexerCommandType m_type;
	std::wstring m_command;
	std::vector<std::wstring> m_arguments;
	FilePath m_projectFilePath;
	FilePath m_databaseFilePath;
	std::wstring m_databaseVersion;
	bool m_runInParallel;
};

#endif	  // INDEXER_COMMAND_CXXL_H
