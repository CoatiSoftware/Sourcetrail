#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QFrame>

#include "utility/file/FilePath.h"
#include "utility/TimePoint.h"
#include "utility/types.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageWindowFocus.h"
#include "qt/utility/QtThreadedFunctor.h"

class QLabel;
class QPushButton;
class QtCodeFileList;
class QtCodeSnippet;
class QVBoxLayout;
class TokenLocationFile;

class QtCodeFile
	: public QFrame
	, MessageListener<MessageWindowFocus>
{
	Q_OBJECT

public:
	QtCodeFile(const FilePath& filePath, QtCodeFileList* parent);
	virtual ~QtCodeFile();

	void setModificationTime(TimePoint modificationTime);

	const FilePath& getFilePath() const;
	std::string getFileName() const;

	const std::vector<Id>& getActiveTokenIds() const;
	const std::vector<Id>& getFocusedTokenIds() const;
	const std::vector<std::string>& getErrorMessages() const;

	void addCodeSnippet(
		uint startLineNumber,
		const std::string& title,
		Id titleId,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		int refCount
	);

	QtCodeSnippet* insertCodeSnippet(
		uint startLineNumber,
		const std::string& title,
		Id titleId,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		int refCount
	);

	QtCodeSnippet* findFirstActiveSnippet() const;
	bool isCollapsedActiveFile() const;

	void updateContent();

	void setLocationFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount);

	void setMinimized();
	void setSnippets();
	void setMaximized();

	void requestSnippets() const;
	bool hasSnippets() const;

public slots:
	void clickedMinimizeButton() const;
	void clickedSnippetButton() const;
	void clickedMaximizeButton() const;

private slots:
	void clickedTitleBar();
	void clickedTitle();

private:
	virtual void handleMessage(MessageWindowFocus* message);

	void updateSnippets();
	void updateRefCount(int refCount);
	void updateTitleBar();
	void doUpdateTitleBar();

	QtThreadedFunctor<> m_updateTitleBarFunctor;

	QtCodeFileList* m_parent;

	QPushButton* m_titleBar;
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
	TimePoint m_modificationTime;

	std::shared_ptr<TokenLocationFile> m_locationFile;
	mutable bool m_snippetsRequested;
};

#endif // QT_CODE_FILE_H
