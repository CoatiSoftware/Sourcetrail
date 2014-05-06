#include "cxxtest/TestSuite.h"

#include <thread>

#include "utility/logging/LogManagerImplementation.h"

class LogManagerTestSuite : public CxxTest::TestSuite
{
public:
	void test_new_logger_can_be_added_to_manager()
	{
		LogManagerImplementation logManagerImplementation;

		std::shared_ptr<Logger> logger = std::make_shared<TestLogger>();

		int countBeforeAdd = logManagerImplementation.getLoggerCount();
		logManagerImplementation.addLogger(logger);
		int countAfterAdd = logManagerImplementation.getLoggerCount();
		logManagerImplementation.removeLogger(logger);

		TS_ASSERT_EQUALS(1, countAfterAdd - countBeforeAdd);
	}

	void test_logger_can_be_removed_from_manager()
	{
		LogManagerImplementation logManagerImplementation;

		std::shared_ptr<Logger> logger = std::make_shared<TestLogger>();

		int countBeforeAdd = logManagerImplementation.getLoggerCount();
		logManagerImplementation.addLogger(logger);
		logManagerImplementation.removeLogger(logger);
		int countAfterRemove = logManagerImplementation.getLoggerCount();

		TS_ASSERT_EQUALS(countBeforeAdd, countAfterRemove);
	}

	void test_logger_logs_message()
	{
		LogManagerImplementation logManagerImplementation;

		std::string log = "test";
		std::shared_ptr<TestLogger> logger = std::make_shared<TestLogger>();

		logManagerImplementation.addLogger(logger);
		logManagerImplementation.logInfo(log, __FILE__, __FUNCTION__, __LINE__);
		int logCount = logger->getMessageCount();
		std::string lastLog = logger->getLastMessage();

		TS_ASSERT_EQUALS(1, logCount);
		TS_ASSERT_EQUALS(log, lastLog);
	}

	void test_logger_logs_warning()
	{
		LogManagerImplementation logManagerImplementation;

		std::string log = "test";
		std::shared_ptr<TestLogger> logger = std::make_shared<TestLogger>();

		logManagerImplementation.addLogger(logger);

		logManagerImplementation.logWarning(log, __FILE__, __FUNCTION__, __LINE__);
		int logCount = logger->getWarningCount();
		std::string lastLog = logger->getLastWarning();

		TS_ASSERT_EQUALS(1, logCount);
		TS_ASSERT_EQUALS(log, lastLog);
	}

	void test_logger_logs_error()
	{
		LogManagerImplementation logManagerImplementation;

		std::string log = "test";
		std::shared_ptr<TestLogger> logger = std::make_shared<TestLogger>();

		logManagerImplementation.addLogger(logger);

		logManagerImplementation.logError(log, __FILE__, __FUNCTION__, __LINE__);
		int logCount = logger->getErrorCount();
		std::string lastLog = logger->getLastError();

		TS_ASSERT_EQUALS(1, logCount);
		TS_ASSERT_EQUALS(log, lastLog);
	}

	void test_new_logger_can_be_added_to_manager_threaded()
	{
		LogManagerImplementation logManagerImplementation;
		unsigned int loggerCount = 100;

		std::thread thread0(addTestLogger, &logManagerImplementation, loggerCount);
		std::thread thread1(addTestLogger, &logManagerImplementation, loggerCount);

		thread0.join();
		thread1.join();

		TS_ASSERT_EQUALS(loggerCount * 2, logManagerImplementation.getLoggerCount());
	}

	void test_logger_can_be_removed_from_manager_threaded()
	{
		LogManagerImplementation logManagerImplementation;
		unsigned int loggerCount = 100;

		std::thread thread0(addAndRemoveTestLogger, &logManagerImplementation, loggerCount);
		std::thread thread1(addAndRemoveTestLogger, &logManagerImplementation, loggerCount);

		thread0.join();
		thread1.join();

		TS_ASSERT_EQUALS(0, logManagerImplementation.getLoggerCount());
	}

	void test_logger_logs_threaded()
	{
		LogManagerImplementation logManagerImplementation;

		std::string log = "foo";
		unsigned int messageCount = 100;
		std::shared_ptr<TestLogger> logger = std::make_shared<TestLogger>();
		logManagerImplementation.addLogger(logger);

		std::thread thread0(logSomeMessages, &logManagerImplementation, log, messageCount);
		std::thread thread1(logSomeMessages, &logManagerImplementation, log, messageCount);

		thread0.join();
		thread1.join();

		TS_ASSERT_EQUALS(logger->getLastError(), log);
		TS_ASSERT_EQUALS(messageCount * 6, logger->getErrorCount() + logger->getWarningCount() + logger->getMessageCount());
	}

private:
	static void addTestLogger(LogManagerImplementation* logManagerImplementation, const unsigned int loggerCount)
	{
		for(unsigned int i = 0; i < loggerCount; i++)
		{
			std::shared_ptr<Logger> logger = std::make_shared<TestLogger>();
			logManagerImplementation->addLogger(logger);
		}
	}

	static void removeTestLoggers(LogManagerImplementation* logManagerImplementation)
	{
		std::shared_ptr<Logger> logger = std::make_shared<TestLogger>();
		logManagerImplementation->removeLoggersByType(logger->getType());
	}

	static void addAndRemoveTestLogger(LogManagerImplementation* logManagerImplementation, const unsigned int loggerCount)
	{
		addTestLogger(logManagerImplementation, loggerCount);
		removeTestLoggers(logManagerImplementation);
	}

	static void logSomeMessages(
		LogManagerImplementation* logManagerImplementation,
		const std::string& message,
		const unsigned int messageCount
	)
	{
		for(unsigned int i = 0; i < messageCount; i++)
		{
			logManagerImplementation->logInfo(message, __FILE__, __FUNCTION__, __LINE__);
			logManagerImplementation->logWarning(message, __FILE__, __FUNCTION__, __LINE__);
			logManagerImplementation->logError(message, __FILE__, __FUNCTION__, __LINE__);
		}
	}

	class TestLogger: public Logger
	{
	public:
		TestLogger();
		~TestLogger();

		void reset();
		int getMessageCount() const;
		int getWarningCount() const;
		int getErrorCount() const;

		std::string getLastMessage() const;
		std::string getLastWarning() const;
		std::string getLastError() const;

		void logInfo(const LogMessage& message);
		void logWarning(const LogMessage& message);
		void logError(const LogMessage& message);

	private:
		int m_logMessageCount;
		int m_logWarningCount;
		int m_logErrorCount;

		std::string m_lastMessage;
		std::string m_lastWarning;
		std::string m_lastError;
	};
};


LogManagerTestSuite::TestLogger::TestLogger()
	: Logger("TestLogger")
	, m_logMessageCount(0)
	, m_logWarningCount(0)
	, m_logErrorCount(0)
	, m_lastMessage("")
	, m_lastWarning("")
	, m_lastError("")
{
}

LogManagerTestSuite::TestLogger::~TestLogger()
{
}

void LogManagerTestSuite::TestLogger::reset()
{
	m_logMessageCount = 0;
	m_logWarningCount = 0;
	m_logErrorCount = 0;
}

int LogManagerTestSuite::TestLogger::getMessageCount() const
{
	return m_logMessageCount;
}

int LogManagerTestSuite::TestLogger::getWarningCount() const
{
	return m_logWarningCount;
}

int LogManagerTestSuite::TestLogger::getErrorCount() const
{
	return m_logErrorCount;
}

std::string LogManagerTestSuite::TestLogger::getLastMessage() const
{
	return m_lastMessage;
}

std::string LogManagerTestSuite::TestLogger::getLastWarning() const
{
	return m_lastWarning;
}

std::string LogManagerTestSuite::TestLogger::getLastError() const
{
	return m_lastError;
}

void LogManagerTestSuite::TestLogger::logInfo(const LogMessage& message)
{
	m_lastMessage = message.message;
	m_logMessageCount++;
}

void LogManagerTestSuite::TestLogger::logWarning(const LogMessage& message)
{
	m_lastWarning = message.message;
	m_logWarningCount++;
}

void LogManagerTestSuite::TestLogger::logError(const LogMessage& message)
{
	m_lastError = message.message;
	m_logErrorCount++;
}
