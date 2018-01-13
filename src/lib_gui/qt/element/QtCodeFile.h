#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QFrame>

#include "utility/file/FilePath.h"

#include "component/view/helper/CodeSnippetParams.h"

class QtCodeArea;
class QtCodeFileTitleBar;
class QtCodeNavigator;
class QtCodeSnippet;
class QVBoxLayout;
class TimeStamp;

class QtCodeFile
	: public QFrame
{
	Q_OBJECT

public:
	QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator);
	virtual ~QtCodeFile();

	void setModificationTime(const TimeStamp modificationTime);

	const FilePath& getFilePath() const;
	std::string getFileName() const;

	const QtCodeFileTitleBar* getTitleBar() const;

	QtCodeSnippet* addCodeSnippet(const CodeSnippetParams& params);
	QtCodeSnippet* insertCodeSnippet(const CodeSnippetParams& params);

	std::vector<QtCodeSnippet*> getVisibleSnippets() const;
	QtCodeSnippet* getSnippetForLocationId(Id locationId) const;
	QtCodeSnippet* getSnippetForLine(unsigned int line) const;
	QtCodeSnippet* getFileSnippet() const;

	std::pair<QtCodeSnippet*, Id> getFirstSnippetWithActiveLocationId(Id tokenId) const;

	bool isCollapsed() const;

	void requestContent();
	void requestWholeFileContent();
	void updateContent();

	void setWholeFile(bool isWholeFile, int refCount);
	void setIsComplete(bool isComplete);

	void setMinimized();
	void setSnippets();
	void setMaximized();

	bool hasSnippets() const;
	void updateSnippets();
	void updateTitleBar();

	void findScreenMatches(const std::string& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches);

public slots:
	void clickedMinimizeButton();
	void clickedSnippetButton();
	void clickedMaximizeButton();

private:
	void updateRefCount(int refCount);

	QtCodeNavigator* m_navigator;

	QtCodeFileTitleBar* m_titleBar;

	QVBoxLayout* m_snippetLayout;
	std::vector<QtCodeSnippet*> m_snippets;
	QtCodeSnippet* m_fileSnippet;

	const FilePath m_filePath;
	bool m_isWholeFile;

	mutable bool m_contentRequested;
};

#endif // QT_CODE_FILE_H
