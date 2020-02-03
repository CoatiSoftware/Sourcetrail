#ifndef ERROR_CONTROLLER_H
#define ERROR_CONTROLLER_H

#include "ActivationListener.h"
#include "MessageErrorCountClear.h"
#include "MessageErrorCountUpdate.h"
#include "MessageErrorsAll.h"
#include "MessageErrorsForFile.h"
#include "MessageErrorsHelpMessage.h"
#include "MessageIndexingFinished.h"
#include "MessageIndexingStarted.h"
#include "MessageListener.h"
#include "MessageShowError.h"
#include "QtThreadedFunctor.h"

#include "Controller.h"
#include "ErrorView.h"

class StorageAccess;

class ErrorController
	: public Controller
	, public ActivationListener
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
	void handleActivation(const MessageActivateBase* message) override;

	void handleMessage(MessageActivateErrors* message) override;
	void handleMessage(MessageErrorCountClear* message) override;
	void handleMessage(MessageErrorCountUpdate* message) override;
	void handleMessage(MessageErrorsAll* message) override;
	void handleMessage(MessageErrorsForFile* message) override;
	void handleMessage(MessageErrorsHelpMessage* message) override;
	void handleMessage(MessageIndexingFinished* message) override;
	void handleMessage(MessageIndexingStarted* message) override;
	void handleMessage(MessageShowError* message) override;

	ErrorView* getView() const;

	void clear() override;

	bool showErrors(const ErrorFilter& filter, bool scrollTo);

	StorageAccess* m_storageAccess;

	size_t m_errorCount = 0;

	std::map<Id, bool> m_tabShowsErrors;
	std::map<Id, FilePath> m_tabActiveFilePath;

	QtThreadedLambdaFunctor m_onQtThread;
	bool m_newErrorsAdded = false;
};

#endif	  // ERROR_CONTROLLER_H
