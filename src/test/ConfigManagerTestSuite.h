#include "cxxtest/TestSuite.h"

#include "utility/ConfigManager.h"


class ConfigManagerTestSuite: public CxxTest::TestSuite
{
public:
	void test_config_manager_returns_true_when_key_is_found()
	{
		std::shared_ptr<ConfigManager> config = ConfigManager::createAndLoad("data/test_config.xml");

		float value;
		bool success = config->getValue("path/to/single_value", value);

		TS_ASSERT(success);
	}


	void test_config_manager_returns_false_when_key_is_not_found()
	{
		std::shared_ptr<ConfigManager> config = ConfigManager::createAndLoad("data/test_config.xml");

		float value;
		bool success = config->getValue("path/to/nowhere", value);

		TS_ASSERT(!success);
	}


	void test_config_manager_returns_correct_string_for_key()
	{
		std::shared_ptr<ConfigManager> config = ConfigManager::createAndLoad("data/test_config.xml");

		std::string value;
		config->getValue("path/to/single_value", value);

		TS_ASSERT_EQUALS("42", value);
	}


	void test_config_manager_returns_correct_float_for_key()
	{
		std::shared_ptr<ConfigManager> config = ConfigManager::createAndLoad("data/test_config.xml");

		float value;
		config->getValue("path/to/single_value", value);

		TS_ASSERT_DELTA(42.0f, value, 0.0001f);
	}

	void test_config_manager_returns_correct_bool_for_key_if_value_is_true()
	{
		std::shared_ptr<ConfigManager> config = ConfigManager::createAndLoad("data/test_config.xml");

		float value;
		bool success(config->getValue("path/to/bool_that_is_true", value));

		TS_ASSERT(success);
		TS_ASSERT(value);
	}

	void test_config_manager_returns_correct_bool_for_key_if_value_is_false()
	{
		std::shared_ptr<ConfigManager> config = ConfigManager::createAndLoad("data/test_config.xml");

		float value;
		bool success(config->getValue("path/to/bool_that_is_false", value));

		TS_ASSERT(success);
		TS_ASSERT(!value);
	}
};
