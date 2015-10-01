#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QFrame>

#include "utility/file/FilePath.h"
#include "utility/types.h"

class QLabel;
class QPushButton;
class QtCodeFileList;
class QtCodeSnippet;
class QVBoxLayout;
class TokenLocationFile;

class QtCodeFile
	: public QFrame
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
		Id titleId,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		int refCount
	);

	QWidget* insertCodeSnippet(
		uint startLineNumber,
		const std::string& title,
		Id titleId,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		int refCount
	);

	QWidget* findFirstActiveSnippet() const;
	bool openCollapsedActiveSnippet() const;

	void updateContent();

	void setLocationFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount);

public slots:
	void clickedSnippetButton();

private slots:
	void clickedTitleBar();
	void clickedTitle();
	void clickedMinimizeButton();
	void clickedMaximizeButton();

private:
	void updateSnippets();
	void updateRefCount(int refCount);

	QtCodeFileList* m_parent;

	QPushButton* m_title;
	QLabel* m_referenceCount;

	QPushButton* m_minimizeButton;
	QPushButton* m_snippetButton;
	QPushButton* m_maximizeButton;

	QVBoxLayout* m_snippetLayout;
	std::vector<std::shared_ptr<QtCodeSnippet>> m_snippets;
	std::shared_ptr<QtCodeSnippet> m_fileSnippet;
	QWidget* m_minimizePlaceholder;

	const FilePath m_filePath;
	std::shared_ptr<TokenLocationFile> m_locationFile;
};

#endif // QT_CODE_FILE_H
