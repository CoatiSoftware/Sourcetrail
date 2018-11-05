#ifndef ERROR_CONTROLLER_H
#define ERROR_CONTROLLER_H

#include "MessageListener.h"
#include "MessageActivateErrors.h"
#include "MessageErrorCountClear.h"
#include "MessageErrorCountUpdate.h"
#include "MessageErrorsAll.h"
#include "MessageErrorsForFile.h"
#include "MessageErrorsHelpMessage.h"
#include "MessageShowError.h"
#include "MessageIndexingFinished.h"
#include "MessageIndexingStarted.h"
#include "MessageActivateAll.h"
#include "MessageActivateFullTextSearch.h"
#include "MessageActivateTokens.h"

#include "Controller.h"
#include "ErrorView.h"

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

	size_t m_errorCount = 0;

	std::map<Id, bool> m_tabShowsErrors;
	std::map<Id, FilePath> m_tabActiveFilePath;
};

#endif // ERROR_CONTROLLER_H
