#ifndef QT_CODE_NAVIGATOR_H
#define QT_CODE_NAVIGATOR_H

#include <QWidget>
#include <QScrollArea>

#include "qt/element/QtCodeFileList.h"
#include "qt/utility/QtScrollSpeedChangeListener.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageCodeReference.h"
#include "utility/messaging/type/MessageWindowFocus.h"

class QLabel;
class QPushButton;
class TokenLocationCollection;
class TokenLocationFile;

class QtCodeNavigator
	: public QWidget
	, public MessageListener<MessageCodeReference>
	, public MessageListener<MessageWindowFocus>
{
	Q_OBJECT

public:
	QtCodeNavigator(QWidget* parent = nullptr);
	virtual ~QtCodeNavigator();

	void addCodeSnippet(const CodeSnippetParams& params, bool insert = false);
	void addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount, TimePoint modificationTime);

	void clearCodeSnippets();

	const std::vector<Id>& getCurrentActiveTokenIds() const;
	void setCurrentActiveTokenIds(const std::vector<Id>& currentActiveTokenIds);

	const std::vector<Id>& getCurrentActiveLocationIds() const;
	void setCurrentActiveLocationIds(const std::vector<Id>& currentActiveLocationIds);

	const std::vector<Id>& getActiveTokenIds() const;
	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);

	const std::vector<Id>& getActiveLocalSymbolIds() const;
	void setActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds);

	const std::vector<Id>& getFocusedTokenIds() const;
	void setFocusedTokenIds(const std::vector<Id>& focusedTokenIds);

	std::string getErrorMessageForId(Id errorId) const;
	void setErrorInfos(const std::vector<ErrorInfo>& errorInfos);

	bool hasErrors() const;
	size_t getFatalErrorCountForFile(const FilePath& filePath) const;

	void showActiveSnippet(
		const std::vector<Id>& activeTokenIds, std::shared_ptr<TokenLocationCollection> collection, bool scrollTo);

	void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	void defocusTokenIds();

	void setFileMinimized(const FilePath path);
	void setFileSnippets(const FilePath path);
	void setFileMaximized(const FilePath path);

	void setupFiles();
	void updateFiles();
	void showContents();

	void showLocation(const FilePath& filePath, Id locationId, bool scrollTo);

	void scrollToValue(int value);
	void scrollToLine(const FilePath& filePath, unsigned int line);
	void scrollToLocation(QtCodeFile* file, Id locationId, bool scrollTo);
	void scrollToDefinition();

	void scrollToSnippetIfRequested();
	void requestScrollToLine(QtCodeFile* file, unsigned int line);

signals:
	void shouldScrollToSnippet(QtCodeSnippet* widget, uint lineNumber, bool onTop);

private slots:
	void scrolled(int value);
	void scrollToSnippet(QtCodeSnippet* snippet, uint lineNumber, bool onTop);
	void setValue();

	void previousReference();
	void nextReference();

private:
	struct Reference
	{
		FilePath filePath;
		Id tokenId;
		Id locationId;
	};

	void showCurrentReference();
	void updateRefLabel();

	void ensureWidgetVisibleAnimated(QWidget *childWidget, QRectF rect, bool onTop);

	void handleMessage(MessageCodeReference* message);
	void handleMessage(MessageWindowFocus* message);

	QtThreadedLambdaFunctor m_onQtThread;

	QScrollArea* m_scrollArea;
	QtCodeFileList* m_list;

	std::vector<Id> m_currentActiveTokenIds;
	std::vector<Id> m_currentActiveLocationIds;

	std::vector<Id> m_activeTokenIds;
	std::vector<Id> m_activeLocalSymbolIds;
	std::vector<Id> m_focusedTokenIds;
	std::map<Id, ErrorInfo> m_errorInfos;

	int m_value;

	QLabel* m_refLabel;
	QPushButton* m_prevButton;
	QPushButton* m_nextButton;

	std::vector<Reference> m_references;
	size_t m_refIndex;

	QtCodeFile* m_scrollToFile;
	uint m_scrollToLine;
	Id m_scrollToLocationId;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListener;
};

#endif // QT_CODE_NAVIGATOR_H
