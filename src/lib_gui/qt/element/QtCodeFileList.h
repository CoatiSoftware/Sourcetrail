#ifndef QT_CODE_FILE_LIST
#define QT_CODE_FILE_LIST

#include <memory>
#include <vector>

#include <QFrame>
#include <QScrollArea>

#include "component/view/helper/CodeSnippetParams.h"
#include "qt/element/QtCodeNavigateable.h"
#include "qt/utility/QtScrollSpeedChangeListener.h"

class QtCodeFile;
class QtCodeNavigator;
class QtCodeSnippet;

class QtCodeFileList
	: public QScrollArea
	, public QtCodeNavigateable
{
	Q_OBJECT

public:
	QtCodeFileList(QtCodeNavigator* navigator);
	virtual ~QtCodeFileList();

	void clear();

	QtCodeFile* getFile(const FilePath filePath);
	void addFile(const FilePath& filePath, bool isWholeFile, int refCount, TimeStamp modificationTime, bool isComplete);

	// QtCodeNaviatebale implementation
	virtual QScrollArea* getScrollArea();

	virtual void addCodeSnippet(const CodeSnippetParams& params);

	virtual void requestFileContent(const FilePath& filePath);
	virtual bool requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, bool onTop);

	virtual void updateFiles();
	virtual void showContents();

	virtual void onWindowFocus();

	virtual void findScreenMatches(const std::string& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches);

	void setFileMinimized(const FilePath path);
	void setFileSnippets(const FilePath path);
	void setFileMaximized(const FilePath path);

	std::pair<QtCodeSnippet*, Id> getFirstSnippetWithActiveLocationId(Id tokenId) const;

private:
	QtCodeNavigator* m_navigator;
	QFrame* m_filesArea;

	std::vector<std::shared_ptr<QtCodeFile>> m_files;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListener;
};

#endif // QT_CODE_FILE_LIST
