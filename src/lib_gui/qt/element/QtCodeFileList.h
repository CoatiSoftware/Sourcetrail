#ifndef QT_CODE_FILE_LIST
#define QT_CODE_FILE_LIST

#include <memory>
#include <vector>

#include <QFrame>
#include <QScrollArea>

#include "utility/file/FilePath.h"
#include "utility/TimePoint.h"
#include "utility/types.h"

class QtCodeFile;
class QtCodeSnippet;
class TokenLocationFile;

class QtCodeFileList
	: public QScrollArea
{
	Q_OBJECT

signals:
	void shouldScrollToSnippet(QtCodeSnippet* widget);

public:
	QtCodeFileList(QWidget* parent = 0);
	virtual ~QtCodeFileList();

	virtual QSize sizeHint() const;

	void addCodeSnippet(
		uint startLineNumber,
		const std::string& title,
		Id titleId,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		int refCount,
		TimePoint modificationTime,
		bool insert = false
	);

	void addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount, TimePoint modificationTime);

	void clearCodeSnippets();

	const std::vector<Id>& getActiveTokenIds() const;
	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);

	const std::vector<Id>& getFocusedTokenIds() const;
	void setFocusedTokenIds(const std::vector<Id>& focusedTokenIds);

	const std::vector<std::string>& getErrorMessages() const;
	void setErrorMessages(const std::vector<std::string>& errorMessages);

	void showActiveTokenIds();

	void showFirstActiveSnippet(bool scrollTo);

	void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	void defocusTokenIds();

	void setFileMinimized(const FilePath path);
	void setFileSnippets(const FilePath path);
	void setFileMaximized(const FilePath path);

	void showContents();
	void scrollToValue(int value);
	void scrollToActiveFileIfRequested();

private slots:
	void scrolled(int value);
	void scrollToSnippet(QtCodeSnippet* snippet);
	void setValue();

private:
	QtCodeFile* getFile(const FilePath filePath);
	QtCodeSnippet* getFirstActiveSnippet() const;

	void updateFiles();

	void expandActiveSnippetFile(bool scrollTo);
	void ensureWidgetVisibleAnimated(QWidget *childWidget, QRectF rect);

	std::shared_ptr<QFrame> m_frame;
	std::vector<std::shared_ptr<QtCodeFile>> m_files;

	std::vector<Id> m_activeTokenIds;
	std::vector<Id> m_focusedTokenIds;
	std::vector<std::string> m_errorMessages;

	QtCodeFile* m_scrollToFile;
	int m_value;
};

#endif // QT_CODE_FILE_LIST
