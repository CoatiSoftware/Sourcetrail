#ifndef QT_CODE_FILE_LIST
#define QT_CODE_FILE_LIST

#include <memory>
#include <vector>

#include <QFrame>
#include <QScrollArea>

#include "utility/TimePoint.h"
#include "utility/types.h"

class QtCodeFile;
class TokenLocationFile;

class QtCodeFileList
	: public QScrollArea
{
	Q_OBJECT

signals:
	void shouldScrollToSnippet(QWidget* widget);

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

	Id getFocusedTokenId() const;

	const std::vector<Id>& getActiveTokenIds() const;
	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);

	const std::vector<std::string>& getErrorMessages() const;
	void setErrorMessages(const std::vector<std::string>& errorMessages);

	bool scrollToFirstActiveSnippet();
	void expandActiveSnippetFile();

	void focusToken(Id tokenId);
	void defocusToken();

private slots:
	void scrollToSnippet(QWidget* widget);

private:
	QtCodeFile* getFile(std::shared_ptr<TokenLocationFile> locationFile);

	void updateFiles();

	void ensureWidgetVisibleAnimated(QWidget *childWidget, int xmargin = 50, int ymargin = 50);

	std::shared_ptr<QFrame> m_frame;
	std::vector<std::shared_ptr<QtCodeFile>> m_files;

	Id m_focusedTokenId;
	std::vector<Id> m_activeTokenIds;
	std::vector<std::string> m_errorMessages;
};

#endif // QT_CODE_FILE_LIST
