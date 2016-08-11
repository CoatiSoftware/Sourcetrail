#include <cxxtest/TestSuite.h>

#include "settings/ProjectSettings.h"
#include "settings/CxxProjectSettings.h"
#include "settings/Settings.h"

class SettingsTestSuite : public CxxTest::TestSuite
{
public:
	void test_settings_get_loaded_from_file()
	{
		TestSettings settings;
		TS_ASSERT(settings.load("data/SettingsTestSuite/settings.xml"));
	}

	void test_settings_get_not_loaded_from_file()
	{
		TestSettings settings;
		TS_ASSERT(!settings.load("data/SettingsTestSuite/wrong_settings.xml"));
	}

	void test_settings_get_loaded_value()
	{
		TestSettings settings;
		TS_ASSERT(settings.load("data/SettingsTestSuite/settings.xml"));

		TS_ASSERT_EQUALS(settings.getBool(), true);
		TS_ASSERT_EQUALS(settings.getInt(), 42);
		TS_ASSERT_EQUALS(settings.getFloat(), 3.1416f);
		TS_ASSERT_EQUALS(settings.getString(), "Hello World!");
	}

	void test_settings_get_default_value_when_not_loaded()
	{
		TestSettings settings;
		TS_ASSERT_EQUALS(settings.getBool(), false);
		TS_ASSERT_EQUALS(settings.getInt(), -1);
		TS_ASSERT_EQUALS(settings.getFloat(), 0.01f);
		TS_ASSERT_EQUALS(settings.getString(), "<empty>");
	}

	void test_settings_get_default_value_when_wrongly_loaded()
	{
		TestSettings settings;
		TS_ASSERT(!settings.load("data/SettingsTestSuite/wrong_settings.xml"));

		TS_ASSERT_EQUALS(settings.getBool(), false);
		TS_ASSERT_EQUALS(settings.getInt(), -1);
		TS_ASSERT_EQUALS(settings.getFloat(), 0.01f);
		TS_ASSERT_EQUALS(settings.getString(), "<empty>");
	}

	void test_settings_get_default_value_after_clearing()
	{
		TestSettings settings;
		TS_ASSERT(settings.load("data/SettingsTestSuite/settings.xml"));

		settings.clear();
		TS_ASSERT_EQUALS(settings.getBool(), false);
		TS_ASSERT_EQUALS(settings.getInt(), -1);
		TS_ASSERT_EQUALS(settings.getFloat(), 0.01f);
		TS_ASSERT_EQUALS(settings.getString(), "<empty>");
	}

	void test_settings_can_be_set_when_not_loaded()
	{
		TestSettings settings;

		TS_ASSERT(settings.setBool(false));
		TS_ASSERT_EQUALS(settings.getBool(), false);

		TS_ASSERT(settings.setInt(2));
		TS_ASSERT_EQUALS(settings.getInt(), 2);

		TS_ASSERT(settings.setFloat(2.5f));
		TS_ASSERT_EQUALS(settings.getFloat(), 2.5f);

		TS_ASSERT(settings.setString("foobar"));
		TS_ASSERT_EQUALS(settings.getString(), "foobar");
	}

	void test_settings_can_be_replaced_when_loaded()
	{
		TestSettings settings;
		TS_ASSERT(settings.load("data/SettingsTestSuite/settings.xml"));

		TS_ASSERT(settings.setBool(false));
		TS_ASSERT_EQUALS(settings.getBool(), false);

		TS_ASSERT(settings.setInt(2));
		TS_ASSERT_EQUALS(settings.getInt(), 2);

		TS_ASSERT(settings.setFloat(2.5f));
		TS_ASSERT_EQUALS(settings.getFloat(), 2.5f);

		TS_ASSERT(settings.setString("foobar"));
		TS_ASSERT_EQUALS(settings.getString(), "foobar");
	}

	void test_settings_can_be_added_when_loaded()
	{
		TestSettings settings;
		TS_ASSERT(settings.load("data/SettingsTestSuite/settings.xml"));

		TS_ASSERT_EQUALS(settings.getNewBool(), false);
		TS_ASSERT(settings.setNewBool(true));
		TS_ASSERT_EQUALS(settings.getNewBool(), true);
	}

	void test_load_project_settings_from_file()
	{
		ProjectSettings settings(FilePath("data/SettingsTestSuite/settings.xml"));
		TS_ASSERT(settings.load());
	}

	void test_load_source_path_from_file()
	{
		ProjectSettings settings(FilePath("data/SettingsTestSuite/settings.xml"));
		settings.load();
		std::vector<FilePath> paths = settings.getSourcePaths();

		TS_ASSERT_EQUALS(paths.size(), 1);
		TS_ASSERT_EQUALS(paths[0].str(), "data");
	}

	void test_load_header_search_paths_from_file()
	{
		CxxProjectSettings settings(FilePath("data/SettingsTestSuite/settings.xml"));
		settings.load();
		std::vector<FilePath> paths = settings.getHeaderSearchPaths();

		TS_ASSERT_EQUALS(paths.size(), 2);
		TS_ASSERT_EQUALS(paths[0].str(), "data/");
		TS_ASSERT_EQUALS(paths[1].str(), "src/");
	}

private:
	class TestSettings
		: public Settings
	{
	public:
		bool getBool() const
		{
			return getValue<bool>("Bool", false);
		}

		bool setBool(bool value)
		{
			return setValue<bool>("Bool", value);
		}

		int getInt() const
		{
			return getValue<int>("Int", -1);
		}

		bool setInt(int value)
		{
			return setValue<int>("Int", value);
		}

		float getFloat() const
		{
			return getValue<float>("Float", 0.01f);
		}

		bool setFloat(float value)
		{
			return setValue<float>("Float", value);
		}

		std::string getString() const
		{
			return getValue<std::string>("String", "<empty>");
		}

		bool setString(const std::string& value)
		{
			return setValue<std::string>("String", value);
		}

		bool getNewBool() const
		{
			return getValue<bool>("NewBool", false);
		}

		bool setNewBool(bool value)
		{
			return setValue<bool>("NewBool", value);
		}
	};
};
