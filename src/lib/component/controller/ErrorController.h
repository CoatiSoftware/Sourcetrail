#ifndef ERROR_CONTROLLER_H
#define ERROR_CONTROLLER_H

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/error/MessageActivateErrors.h"
#include "utility/messaging/type/error/MessageErrorCountClear.h"
#include "utility/messaging/type/error/MessageErrorCountUpdate.h"
#include "utility/messaging/type/error/MessageErrorsAll.h"
#include "utility/messaging/type/error/MessageErrorsForFile.h"
#include "utility/messaging/type/error/MessageErrorsHelpMessage.h"
#include "utility/messaging/type/error/MessageShowError.h"
#include "utility/messaging/type/indexing/MessageIndexingFinished.h"
#include "utility/messaging/type/indexing/MessageIndexingStarted.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateFullTextSearch.h"
#include "utility/messaging/type/MessageActivateTokens.h"

#include "component/controller/Controller.h"
#include "component/view/ErrorView.h"

class StorageAccess;

class ErrorController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateFullTextSearch>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageErrorCountClear>
	, public MessageListener<MessageErrorCountUpdate>
	, public MessageListener<MessageErrorsAll>
	, public MessageListener<MessageErrorsForFile>
	, public MessageListener<MessageErrorsHelpMessage>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageIndexingStarted>
	, public MessageListener<MessageShowError>
{
public:
	ErrorController(StorageAccess* storageAccess);
	~ErrorController();

	void errorFilterChanged(const ErrorFilter& filter);
	void showError(Id errorId);

private:
	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateErrors* message);
	virtual void handleMessage(MessageActivateFullTextSearch* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageErrorCountClear* message);
	virtual void handleMessage(MessageErrorCountUpdate* message);
	virtual void handleMessage(MessageErrorsAll* message);
	virtual void handleMessage(MessageErrorsForFile* message);
	virtual void handleMessage(MessageErrorsHelpMessage* message);
	virtual void handleMessage(MessageIndexingFinished* message);
	virtual void handleMessage(MessageIndexingStarted* message);
	virtual void handleMessage(MessageShowError* message);

	ErrorView* getView() const;

	virtual void clear();

	bool showErrors(const ErrorFilter& filter, bool scrollTo);
	bool canDisplayErrors() const;

	StorageAccess* m_storageAccess;

	bool m_showsErrors = false;
	size_t m_errorCount = 0;
	FilePath m_activeFilePath;
};

#endif // ERROR_CONTROLLER_H
