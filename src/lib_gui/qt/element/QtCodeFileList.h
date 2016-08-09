#ifndef QT_CODE_FILE_LIST
#define QT_CODE_FILE_LIST

#include <memory>
#include <vector>

#include <QFrame>

#include "utility/file/FilePath.h"
#include "utility/TimePoint.h"
#include "utility/types.h"

#include "data/ErrorInfo.h"
#include "component/view/helper/CodeSnippetParams.h"

class QtCodeFile;
class QtCodeNavigator;
class QtCodeSnippet;
class TokenLocationFile;

class QtCodeFileList
	: public QFrame
{
	Q_OBJECT

signals:
	void shouldScrollToSnippet(QtCodeSnippet* widget, uint lineNumber);

public:
	QtCodeFileList(QtCodeNavigator* navigator);
	virtual ~QtCodeFileList();

	void addCodeSnippet(const CodeSnippetParams& params, bool insert = false);
	void addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount, TimePoint modificationTime);

	void clearCodeSnippets();

	void showLocation(const FilePath& filePath, Id locationId, bool scrollTo);

	void requestFileContent(const FilePath& filePath);

	void setFileMinimized(const FilePath path);
	void setFileSnippets(const FilePath path);
	void setFileMaximized(const FilePath path);

	void updateFiles();
	void showContents();

	void scrollToLocation(QtCodeFile* file, Id locationId, bool scrollTo);
	void scrollToLine(std::string filename, unsigned int line);
	void scrollToSnippetIfRequested();

private:
	QtCodeFile* getFile(const FilePath filePath);
	QtCodeSnippet* getFirstActiveSnippet() const;

	void expandActiveSnippetFile(bool scrollTo);

	QtCodeNavigator* m_navigator;

	std::vector<std::shared_ptr<QtCodeFile>> m_files;

	QtCodeFile* m_scrollToFile;
	Id m_scrollToLocationId;
};

#endif // QT_CODE_FILE_LIST
