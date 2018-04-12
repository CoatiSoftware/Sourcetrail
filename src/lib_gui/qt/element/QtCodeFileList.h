#ifndef QT_CODE_FILE_LIST
#define QT_CODE_FILE_LIST

#include <vector>

#include <QFrame>
#include <QScrollArea>

#include "component/view/helper/CodeSnippetParams.h"
#include "qt/element/QtCodeNavigateable.h"
#include "qt/utility/QtScrollSpeedChangeListener.h"

class QtCodeFile;
class QtCodeFileTitleBar;
class QtCodeNavigator;
class QtCodeSnippet;

class QtCodeFileList
	: public QFrame
	, public QtCodeNavigateable
{
	Q_OBJECT

public:
	QtCodeFileList(QtCodeNavigator* navigator);
	virtual ~QtCodeFileList();

	void clear();
	void clearSnippetTitleAndScrollBar();

	QtCodeFile* getFile(const FilePath filePath);
	void addFile(const FilePath& filePath, bool isWholeFile, int refCount, TimeStamp modificationTime, bool isComplete);

	// QtCodeNaviatebale implementation
	virtual QScrollArea* getScrollArea();

	virtual void addCodeSnippet(const CodeSnippetParams& params);
	virtual void updateCodeSnippet(const CodeSnippetParams& params);

	virtual void requestFileContent(const FilePath& filePath);
	virtual bool requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, ScrollTarget target);

	virtual void updateFiles();
	virtual void showContents();

	virtual void onWindowFocus();

	virtual void findScreenMatches(const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches);

	void setFileMinimized(const FilePath path);
	void setFileSnippets(const FilePath path);
	void setFileMaximized(const FilePath path);

	void maximizeFirstFile();

	std::pair<QtCodeSnippet*, Id> getFirstSnippetWithActiveLocationId(Id tokenId) const;

protected:
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void updateSnippetTitleAndScrollBarSlot();
	void updateSnippetTitleAndScrollBar(int value = 0);
	void scrollLastSnippet(int value);
	void scrollLastSnippetScrollBar(int value);

private:
	void updateFirstSnippetTitleBar(const QtCodeFile* file, int fileTitleBarOffset = 0);
	void updateLastSnippetScrollBar(QScrollBar* mirroredScrollBar);

	QtCodeNavigator* m_navigator;
	QScrollArea* m_scrollArea;
	QFrame* m_filesArea;

	std::vector<QtCodeFile*> m_files;

	QtCodeFileTitleBar* m_firstSnippetTitleBar;
	const QtCodeFileTitleBar* m_mirroredTitleBar;

	QScrollBar* m_lastSnippetScrollBar;
	QScrollBar* m_mirroredSnippetScrollBar;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListener;
	int m_styleSize = 0;
};

#endif // QT_CODE_FILE_LIST
