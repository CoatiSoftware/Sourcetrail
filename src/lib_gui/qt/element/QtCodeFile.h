#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QFrame>

#include "utility/file/FilePath.h"
#include "utility/TimePoint.h"
#include "utility/types.h"

#include "data/ErrorInfo.h"
#include "component/view/helper/CodeSnippetParams.h"

class QLabel;
class QPushButton;
class QtCodeNavigator;
class QtCodeSnippet;
class QVBoxLayout;
class TokenLocationFile;

class QtCodeFile
	: public QFrame
{
	Q_OBJECT

public:
	QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator);
	virtual ~QtCodeFile();

	void setModificationTime(TimePoint modificationTime);

	const FilePath& getFilePath() const;
	std::string getFileName() const;

	QtCodeSnippet* addCodeSnippet(const CodeSnippetParams& params);
	QtCodeSnippet* insertCodeSnippet(const CodeSnippetParams& params);

	QtCodeSnippet* getSnippetForLocationId(Id locationId) const;
	QtCodeSnippet* getSnippetForLine(unsigned int line) const;
	QtCodeSnippet* getFileSnippet() const;

	std::pair<QtCodeSnippet*, int> getFirstSnippetWithActiveScope() const;

	bool isCollapsed() const;

	void requestContent() const;
	void updateContent();

	void setLocationFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount);

	void setMinimized();
	void setSnippets();
	void setMaximized();

	bool hasSnippets() const;
	void updateSnippets();
	void updateTitleBar();

public slots:
	void clickedMinimizeButton() const;
	void clickedSnippetButton() const;
	void clickedMaximizeButton() const;

private slots:
	void clickedTitleBar();
	void clickedTitle();
	void editProject();

private:
	void updateRefCount(int refCount);

	QtCodeNavigator* m_navigator;

	QPushButton* m_titleBar;
	QPushButton* m_title;
	QLabel* m_referenceCount;

	QPushButton* m_minimizeButton;
	QPushButton* m_snippetButton;
	QPushButton* m_maximizeButton;

	QVBoxLayout* m_snippetLayout;
	std::vector<std::shared_ptr<QtCodeSnippet>> m_snippets;
	std::shared_ptr<QtCodeSnippet> m_fileSnippet;

	const FilePath m_filePath;
	TimePoint m_modificationTime;

	std::shared_ptr<TokenLocationFile> m_locationFile;
	mutable bool m_contentRequested;
};

#endif // QT_CODE_FILE_H
