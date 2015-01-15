#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QWidget>

#include "utility/types.h"

class QtCodeFileList;
class QtCodeSnippet;
class TokenLocationFile;

class QtCodeFile : public QWidget
{
public:
	QtCodeFile(const std::string& filePath, QtCodeFileList* parent);
	virtual ~QtCodeFile();

	const std::string& getFilePath() const;
	std::string getFileName() const;
	const std::vector<Id>& getActiveTokenIds() const;
	const std::vector<std::string>& getErrorMessages() const;

	void addCodeSnippet(
		uint startLineNumber,
		const std::string& code,
		const TokenLocationFile& locationFile
	);

	void update();

private:
	QtCodeFileList* m_parent;

	std::vector<std::shared_ptr<QtCodeSnippet>> m_snippets;
	const std::string m_filePath;
};

#endif // QT_CODE_FILE_H
