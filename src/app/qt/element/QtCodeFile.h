#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QWidget>

#include "utility/file/FilePath.h"
#include "utility/types.h"

class QPushButton;
class QtCodeFileList;
class QtCodeSnippet;
class TokenLocationFile;

class QtCodeFile
	: public QWidget
{
	Q_OBJECT

public:
	QtCodeFile(const FilePath& filePath, QtCodeFileList* parent);
	virtual ~QtCodeFile();

	const FilePath& getFilePath() const;
	std::string getFileName() const;
	Id getFocusedTokenId() const;
	const std::vector<Id>& getActiveTokenIds() const;
	const std::vector<std::string>& getErrorMessages() const;

	void addCodeSnippet(
		uint startLineNumber,
		const std::string& title,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile
	);

	void updateContent();

private slots:
	void clickedTitle();

private:
	QtCodeFileList* m_parent;

	QPushButton* m_title;

	std::vector<std::shared_ptr<QtCodeSnippet>> m_snippets;
	const FilePath m_filePath;
};

#endif // QT_CODE_FILE_H
