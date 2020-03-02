#ifndef QT_CODE_FILE_LIST
#define QT_CODE_FILE_LIST

#include <vector>

#include <QFrame>
#include <QScrollArea>

#include "CodeSnippetParams.h"
#include "QtCodeNavigateable.h"
#include "QtScrollSpeedChangeListener.h"

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
	virtual ~QtCodeFileList() = default;

	void clear();
	void clearSnippetTitleAndScrollBar();

	QtCodeFile* getFile(const FilePath& filePath);

	void addFile(const CodeFileParams& params);

	// QtCodeNaviatebale implementation
	QScrollArea* getScrollArea() override;

	void updateSourceLocations(const CodeSnippetParams& params) override;
	void updateFiles() override;

	void scrollTo(
		const FilePath& filePath,
		size_t lineNumber,
		Id locationId,
		bool animated,
		CodeScrollParams::Target target) override;

	void onWindowFocus() override;

	void findScreenMatches(
		const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches) override;

	void maximizeFirstFile();

	std::pair<QtCodeFile*, Id> getFirstFileWithActiveLocationId() const;
	std::pair<QtCodeSnippet*, Id> getFirstSnippetWithActiveLocationId(Id tokenId) const;

protected:
	void resizeEvent(QResizeEvent* event) override;

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

#endif	  // QT_CODE_FILE_LIST
