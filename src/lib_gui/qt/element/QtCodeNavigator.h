#ifndef QT_CODE_NAVIGATOR_H
#define QT_CODE_NAVIGATOR_H

#include <QWidget>

#include "ErrorInfo.h"
#include "LocationType.h"
#include "QtCodeFileList.h"
#include "QtCodeFileSingle.h"
#include "QtThreadedFunctor.h"
#include "MessageListener.h"
#include "MessageIndexingFinished.h"
#include "MessageCodeReference.h"
#include "MessageShowReference.h"
#include "MessageSwitchColorScheme.h"
#include "MessageWindowFocus.h"

class QLabel;
class QPushButton;
class QtSearchBarButton;
class SourceLocationCollection;
class SourceLocationFile;

class QtCodeNavigator
	: public QWidget
	, public MessageListener<MessageCodeReference>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageShowReference>
	, public MessageListener<MessageSwitchColorScheme>
	, public MessageListener<MessageWindowFocus>
{
	Q_OBJECT

public:
	enum Mode
	{
		MODE_NONE,
		MODE_LIST,
		MODE_SINGLE
	};

	QtCodeNavigator(QWidget* parent = nullptr);
	virtual ~QtCodeNavigator();

	void addCodeSnippet(const CodeSnippetParams& params);
	void updateCodeSnippet(const CodeSnippetParams& params);
	void addFile(std::shared_ptr<SourceLocationFile> locationFile, int refCount, TimeStamp modificationTime);

	void addedFiles();

	void clear();
	void clearCodeSnippets(bool useSingleFileCache);
	void clearFile();
	void clearCaches();
	void clearSnippetReferences();

	void setMode(Mode mode);

	Id getSchedulerId() const override;
	void setSchedulerId(Id schedulerId);

	const std::set<Id>& getCurrentActiveTokenIds() const;
	void setCurrentActiveTokenIds(const std::vector<Id>& currentActiveTokenIds);

	const std::set<Id>& getCurrentActiveLocationIds() const;
	void setCurrentActiveLocationIds(const std::vector<Id>& currentActiveLocationIds);

	const std::set<Id>& getCurrentActiveLocalLocationIds() const;
	void setCurrentActiveLocalLocationIds(const std::vector<Id>& currentActiveLocalLocationIds);

	const std::set<Id>& getActiveTokenIds() const;
	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);

	const std::set<Id>& getActiveLocalTokenIds() const;
	void setActiveLocalTokenIds(const std::vector<Id>& activeLocalTokenIds, LocationType locationType);

	const std::set<Id>& getFocusedTokenIds() const;
	void setFocusedTokenIds(const std::vector<Id>& focusedTokenIds);

	std::wstring getErrorMessageForId(Id errorId) const;
	void setErrorInfos(const std::vector<ErrorInfo>& errorInfos);

	bool hasErrors() const;
	size_t getFatalErrorCountForFile(const FilePath& filePath) const;

	bool isInListMode() const;
	bool hasSingleFileCached(const FilePath& filePath) const;

	void showActiveSnippet(
		const std::vector<Id>& activeTokenIds, std::shared_ptr<SourceLocationCollection> collection, bool scrollTo);

	void focusTokenIds(const std::vector<Id>& focusedTokenIds);
	void defocusTokenIds();

	void setFileMinimized(const FilePath path);
	void setFileSnippets(const FilePath path);
	void setFileMaximized(const FilePath path);

	void updateFiles();
	void showContents();

	void refreshStyle();

	size_t findScreenMatches(const std::wstring& query);
	void activateScreenMatch(size_t matchIndex);
	void deactivateScreenMatch(size_t matchIndex);
	bool hasScreenMatches() const;
	void clearScreenMatches();

	void scrollToValue(int value, bool inListMode);
	void scrollToLine(const FilePath& filePath, unsigned int line);
	void scrollToDefinition(bool animated, bool ignoreActiveReference);

	void scrollToSnippetIfRequested();

	void requestScroll(
		const FilePath& filePath, uint lineNumber, Id locationId, bool animated, QtCodeNavigateable::ScrollTarget target);

signals:
	void scrollRequest();

public slots:
	void scrolled(int value);

protected:
	void showEvent(QShowEvent* event) override;

private slots:
	void handleScrollRequest();
	void setValue();

	void previousFile(bool fromUI = true);
	void nextFile(bool fromUI = true);

	void previousReference(bool fromUI = true);
	void nextReference(bool fromUI = true);

	void previousLocalReference(bool fromUI = true);
	void nextLocalReference(bool fromUI = true);

	void setModeList();
	void setModeSingle();

private:
	struct Reference
	{
		Reference()
			: tokenId(0)
			, locationId(0)
			, locationType(LOCATION_TOKEN)
		{
		}

		FilePath filePath;
		Id tokenId;
		Id locationId;
		LocationType locationType;
	};

	void showCurrentReference(bool fromUI);
	void showCurrentLocalReference();
	void updateRefLabels();

	struct ScrollRequest
	{
		ScrollRequest()
			: lineNumber(0)
			, locationId(0)
			, animated(false)
			, target(QtCodeNavigateable::SCROLL_VISIBLE)
		{
		}

		FilePath filePath;
		uint lineNumber;
		Id locationId;

		bool animated;
		QtCodeNavigateable::ScrollTarget target;
	};

	void handleMessage(MessageCodeReference* message) override;
	void handleMessage(MessageIndexingFinished* message) override;
	void handleMessage(MessageShowReference* message) override;
	void handleMessage(MessageSwitchColorScheme* message) override;
	void handleMessage(MessageWindowFocus* message) override;

	QtThreadedLambdaFunctor m_onQtThread;

	QtCodeNavigateable* m_current;
	QtCodeFileList* m_list;
	QtCodeFileSingle* m_single;

	Mode m_mode;
	Mode m_oldMode;

	Id m_schedulerId;

	std::set<Id> m_currentActiveTokenIds;
	std::set<Id> m_currentActiveLocationIds;
	std::set<Id> m_currentActiveLocalLocationIds;

	std::set<Id> m_activeTokenIds;
	std::set<Id> m_activeLocalTokenIds;
	std::set<Id> m_focusedTokenIds;
	std::map<Id, ErrorInfo> m_errorInfos;

	Id m_activeTokenId;

	int m_value;

	QtSearchBarButton* m_prevFileButton;
	QtSearchBarButton* m_nextFileButton;
	QtSearchBarButton* m_prevReferenceButton;
	QtSearchBarButton* m_nextReferenceButton;
	QLabel* m_refLabel;

	QtSearchBarButton* m_prevLocalReferenceButton;
	QtSearchBarButton* m_nextLocalReferenceButton;
	QLabel* m_localRefLabel;

	QtSearchBarButton* m_listButton;
	QtSearchBarButton* m_fileButton;

	QFrame* m_separatorLine;

	std::vector<Reference> m_references;
	Reference m_activeReference;
	size_t m_refIndex;

	std::vector<Reference> m_localReferences;
	size_t m_localRefIndex;

	ScrollRequest m_scrollRequest;
	bool m_singleHasNewFile;
	bool m_useSingleFileCache;

	std::vector<std::pair<QtCodeArea*, Id>> m_screenMatches;
	Id m_activeScreenMatchId = 0;
};

#endif // QT_CODE_NAVIGATOR_H
