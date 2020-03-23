#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <set>
#include <string>
#include <vector>

#include <QFrame>

#include "FilePath.h"

#include "CodeFocusHandler.h"
#include "CodeSnippetParams.h"

class QtCodeArea;
class QtCodeFileTitleBar;
class QtCodeNavigator;
class QtCodeSnippet;
class QVBoxLayout;
class TimeStamp;

class QtCodeFile: public QFrame
{
	Q_OBJECT

public:
	QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator, bool isFirst);
	virtual ~QtCodeFile();

	void setModificationTime(const TimeStamp modificationTime);

	const FilePath& getFilePath() const;

	const QtCodeFileTitleBar* getTitleBar() const;

	QtCodeSnippet* addCodeSnippet(const CodeSnippetParams& params);
	void updateSourceLocations(const CodeSnippetParams& params);

	const std::vector<QtCodeSnippet*>& getSnippets() const;
	std::vector<QtCodeSnippet*> getVisibleSnippets() const;
	QtCodeSnippet* getSnippetForLocationId(Id locationId) const;
	QtCodeSnippet* getSnippetForLine(unsigned int line) const;

	std::pair<QtCodeSnippet*, Id> getFirstSnippetWithActiveLocationId(Id tokenId) const;

	void requestWholeFileContent(size_t targetLineNumber);
	void updateContent();

	void setWholeFile(bool isWholeFile, int refCount);
	void setIsComplete(bool isComplete);
	void setIsIndexed(bool isIndexed);

	bool isCollapsed() const;
	void toggleCollapsed();

	void setMinimized();
	void setSnippets();

	bool hasSnippets() const;
	void clearSnippets();
	void updateSnippets();
	void updateTitleBar();

	void findScreenMatches(
		const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches);

	bool hasFocus(const CodeFocusHandler::Focus& focus) const;
	bool setFocus(Id locationId);
	bool moveFocus(const CodeFocusHandler::Focus& focus, CodeFocusHandler::Direction direction);
	void focusTop();
	void focusBottom();

	void copySelection();

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

	const FilePath m_filePath;
	bool m_isWholeFile;
};

#endif	  // QT_CODE_FILE_H
