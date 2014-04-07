#include "cxxtest/TestSuite.h"

#include "utility/logging/logging.h"

class LogManagerTestSuite : public CxxTest::TestSuite
{
public:
	void test_new_logger_can_be_added_to_manager()
	{
		std::shared_ptr<Logger> logger = std::make_shared<TestLogger>();

		int countBeforeAdd = LogManager::getInstance()->getLoggerCount();
		LogManager::getInstance()->addLogger(logger);
		int countAfterAdd = LogManager::getInstance()->getLoggerCount();
		LogManager::getInstance()->removeLogger(logger);

		TS_ASSERT_EQUALS(1, countAfterAdd - countBeforeAdd);
	}

	void test_logger_can_be_removed_from_manager()
	{
		std::shared_ptr<Logger> logger = std::make_shared<TestLogger>();

		int countBeforeAdd = LogManager::getInstance()->getLoggerCount();
		LogManager::getInstance()->addLogger(logger);
		LogManager::getInstance()->removeLogger(logger);
		int countAfterRemove = LogManager::getInstance()->getLoggerCount();

		TS_ASSERT_EQUALS(countBeforeAdd, countAfterRemove);
	}

	void test_logger_logs_message()
	{
		std::string log = "test";
		std::shared_ptr<TestLogger> logger = std::make_shared<TestLogger>();

		LogManager::getInstance()->addLogger(logger);

		LOG_INFO(log);
		int logCount = logger->getMessageCount();
		std::string lastLog = logger->getLastMessage();

		LogManager::getInstance()->removeLogger(logger);

		TS_ASSERT_EQUALS(1, logCount);
		TS_ASSERT_EQUALS(log, lastLog);
	}

	void test_logger_logs_warning()
	{
		std::string log = "test";
		std::shared_ptr<TestLogger> logger = std::make_shared<TestLogger>();

		LogManager::getInstance()->addLogger(logger);

		LOG_WARNING(log);
		int logCount = logger->getWarningCount();
		std::string lastLog = logger->getLastWarning();

		LogManager::getInstance()->removeLogger(logger);

		TS_ASSERT_EQUALS(1, logCount);
		TS_ASSERT_EQUALS(log, lastLog);
	}

	void test_logger_logs_error()
	{
		std::string log = "test";
		std::shared_ptr<TestLogger> logger = std::make_shared<TestLogger>();

		LogManager::getInstance()->addLogger(logger);

		LOG_ERROR(log);
		int logCount = logger->getErrorCount();
		std::string lastLog = logger->getLastError();

		LogManager::getInstance()->removeLogger(logger);

		TS_ASSERT_EQUALS(1, logCount);
		TS_ASSERT_EQUALS(log, lastLog);
	}

private:
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
