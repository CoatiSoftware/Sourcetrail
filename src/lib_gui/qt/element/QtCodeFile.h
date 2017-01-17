#ifndef QT_CODE_FILE_H
#define QT_CODE_FILE_H

#include <memory>
#include <string>
#include <vector>

#include <QFrame>

#include "component/view/helper/CodeSnippetParams.h"
#include "utility/file/FilePath.h"

class QLabel;
class QPushButton;
class QtCodeFileTitleButton;
class QtCodeNavigator;
class QtCodeSnippet;
class QVBoxLayout;
class TimePoint;

class QtCodeFile
	: public QFrame
{
	Q_OBJECT

public:
	QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator);
	virtual ~QtCodeFile();

	void setModificationTime(const TimePoint modificationTime);

	const FilePath& getFilePath() const;
	std::string getFileName() const;

	QtCodeSnippet* addCodeSnippet(const CodeSnippetParams& params);
	QtCodeSnippet* insertCodeSnippet(const CodeSnippetParams& params);

	QtCodeSnippet* getSnippetForLocationId(Id locationId) const;
	QtCodeSnippet* getSnippetForLine(unsigned int line) const;
	QtCodeSnippet* getFileSnippet() const;

	std::pair<QtCodeSnippet*, uint> getFirstSnippetWithActiveLocation(Id tokenId) const;

	bool isCollapsed() const;

	void requestContent();
	void updateContent();

	void setWholeFile(bool isWholeFile, int refCount);

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

private:
	void updateRefCount(int refCount);

	QtCodeNavigator* m_navigator;

	QPushButton* m_titleBar;
	QtCodeFileTitleButton* m_title;
	QLabel* m_referenceCount;

	QPushButton* m_minimizeButton;
	QPushButton* m_snippetButton;
	QPushButton* m_maximizeButton;

	QVBoxLayout* m_snippetLayout;
	std::vector<std::shared_ptr<QtCodeSnippet>> m_snippets;
	std::shared_ptr<QtCodeSnippet> m_fileSnippet;

	const FilePath m_filePath;

	bool m_isWholeFile;
	bool m_isCollapsed;

	mutable bool m_contentRequested;
};

#endif // QT_CODE_FILE_H
