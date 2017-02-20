#ifndef QT_CODE_NAVIGATOR_H
#define QT_CODE_NAVIGATOR_H

#include <QWidget>

#include "data/ErrorInfo.h"
#include "data/location/LocationType.h"
#include "qt/element/QtCodeFileList.h"
#include "qt/element/QtCodeFileSingle.h"
#include "qt/utility/QtThreadedFunctor.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageCodeReference.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageSwitchColorScheme.h"
#include "utility/messaging/type/MessageWindowFocus.h"

class QLabel;
class QPushButton;
class TokenLocationCollection;
class TokenLocationFile;

class QtCodeNavigator
	: public QWidget
	, public MessageListener<MessageCodeReference>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageSwitchColorScheme>
	, public MessageListener<MessageWindowFocus>
{
	Q_OBJECT

public:
	QtCodeNavigator(QWidget* parent = nullptr);
	virtual ~QtCodeNavigator();

	void addCodeSnippet(const CodeSnippetParams& params, bool insert = false);
	void addFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount, TimePoint modificationTime);

	void addedFiles();

	void clear();
	void clearCodeSnippets();
	void clearCaches();

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

	void refreshStyle();

	void scrollToValue(int value, bool inListMode);
	void scrollToLine(const FilePath& filePath, unsigned int line);
	void scrollToDefinition(bool ignoreActiveReference);

	void scrollToSnippetIfRequested();

	void requestScroll(const FilePath& filePath, uint lineNumber, Id locationId, bool animated, bool onTop);

signals:
	void scrollRequest();

private slots:
	void handleScrollRequest();
	void scrolled(int value);
	void setValue();

	void previousReference(bool fromUI = true);
	void nextReference(bool fromUI = true);

	void setModeList();
	void setModeSingle();

private:
	enum Mode
	{
		MODE_NONE,
		MODE_LIST,
		MODE_SINGLE
	};

	void setMode(Mode mode);

	struct Reference
	{
		Reference()
			: tokenId(0)
			, locationId(0)
		{
		}

		FilePath filePath;
		Id tokenId;
		Id locationId;
		LocationType locationType;
	};

	void showCurrentReference(bool fromUI);
	void updateRefLabel();

	struct ScrollRequest
	{
		ScrollRequest()
			: lineNumber(0)
			, locationId(0)
			, animated(false)
			, onTop(false)
		{
		}

		FilePath filePath;
		uint lineNumber;
		Id locationId;

		bool animated;
		bool onTop;
	};

	void handleMessage(MessageCodeReference* message);
	void handleMessage(MessageFinishedParsing* message);
	void handleMessage(MessageSwitchColorScheme* message);
	void handleMessage(MessageWindowFocus* message);

	QtThreadedLambdaFunctor m_onQtThread;

	QtCodeNavigateable* m_current;
	QtCodeFileList* m_list;
	QtCodeFileSingle* m_single;

	Mode m_mode;

	std::vector<Id> m_currentActiveTokenIds;
	std::vector<Id> m_currentActiveLocationIds;

	std::vector<Id> m_activeTokenIds;
	std::vector<Id> m_activeLocalSymbolIds;
	std::vector<Id> m_focusedTokenIds;
	std::map<Id, ErrorInfo> m_errorInfos;

	int m_value;

	QPushButton* m_listButton;
	QPushButton* m_fileButton;
	QLabel* m_refLabel;
	QPushButton* m_prevButton;
	QPushButton* m_nextButton;
	QFrame* m_separatorLine;

	std::vector<Reference> m_references;
	Reference m_activeReference;
	size_t m_refIndex;

	ScrollRequest m_scrollRequest;
	bool m_singleHasNewFile;
};

#endif // QT_CODE_NAVIGATOR_H
