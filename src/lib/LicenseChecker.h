#ifndef LICENSE_CHECKER_H
#define LICENSE_CHECKER_H

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageEnteredLicense.h"
#include "utility/messaging/type/MessageDispatchWhenLicenseValid.h"

class Application;

class LicenseChecker
	: public MessageListener<MessageDispatchWhenLicenseValid>
	, public MessageListener<MessageEnteredLicense>
{
public:
	static void createInstance();
	static std::shared_ptr<LicenseChecker> getInstance();

	~LicenseChecker();

	void setApp(Application* app);

private:
	LicenseChecker();
	LicenseChecker(const LicenseChecker&) = delete;
	void operator=(const LicenseChecker&) = delete;

	void handleMessage(MessageDispatchWhenLicenseValid* message);
	void handleMessage(MessageEnteredLicense* message);
	bool checkLicenseString();

	static std::shared_ptr<LicenseChecker> s_instance;

	Application* m_app;
	std::shared_ptr<MessageBase> m_pendingMessage;
	bool m_forcedLicenseEntering;
};

#endif // LICENSE_CHECKER_H
