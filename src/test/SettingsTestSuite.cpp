#include "catch.hpp"

#include "ProjectSettings.h"
#include "Settings.h"
#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithSourcePaths.h"

namespace
{
class TestSettings: public Settings
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

	std::wstring getWString() const
	{
		return getValue<std::wstring>("WString", L"<empty>");
	}

	bool setWString(const std::wstring& value)
	{
		return setValue<std::wstring>("WString", value);
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
}	 // namespace

TEST_CASE("settings get loaded from file")
{
	TestSettings settings;
	REQUIRE(settings.load(FilePath(L"data/SettingsTestSuite/settings.xml")));
}

TEST_CASE("settings get not loaded from file")
{
	TestSettings settings;
	REQUIRE(!settings.load(FilePath(L"data/SettingsTestSuite/wrong_settings.xml")));
}

TEST_CASE("settings get loaded value")
{
	TestSettings settings;
	REQUIRE(settings.load(FilePath(L"data/SettingsTestSuite/settings.xml")));

	REQUIRE(settings.getBool() == true);
	REQUIRE(settings.getInt() == 42);
	REQUIRE(settings.getFloat() == 3.1416f);
	REQUIRE(settings.getString() == "Hello World!");
	REQUIRE(settings.getWString() == L"Hello World!");
}

TEST_CASE("settings get default value when not loaded")
{
	TestSettings settings;
	REQUIRE(settings.getBool() == false);
	REQUIRE(settings.getInt() == -1);
	REQUIRE(settings.getFloat() == 0.01f);
	REQUIRE(settings.getString() == "<empty>");
	REQUIRE(settings.getWString() == L"<empty>");
}

TEST_CASE("settings get default value when wrongly loaded")
{
	TestSettings settings;
	REQUIRE(!settings.load(FilePath(L"data/SettingsTestSuite/wrong_settings.xml")));

	REQUIRE(settings.getBool() == false);
	REQUIRE(settings.getInt() == -1);
	REQUIRE(settings.getFloat() == 0.01f);
	REQUIRE(settings.getString() == "<empty>");
	REQUIRE(settings.getWString() == L"<empty>");
}

TEST_CASE("settings get default value after clearing")
{
	TestSettings settings;
	REQUIRE(settings.load(FilePath(L"data/SettingsTestSuite/settings.xml")));

	settings.clear();
	REQUIRE(settings.getBool() == false);
	REQUIRE(settings.getInt() == -1);
	REQUIRE(settings.getFloat() == 0.01f);
	REQUIRE(settings.getString() == "<empty>");
	REQUIRE(settings.getWString() == L"<empty>");
}

TEST_CASE("settings can be set when not loaded")
{
	TestSettings settings;

	REQUIRE(settings.setBool(false));
	REQUIRE(settings.getBool() == false);

	REQUIRE(settings.setInt(2));
	REQUIRE(settings.getInt() == 2);

	REQUIRE(settings.setFloat(2.5f));
	REQUIRE(settings.getFloat() == 2.5f);

	REQUIRE(settings.setString("foo"));
	REQUIRE(settings.getString() == "foo");

	REQUIRE(settings.setWString(L"bar"));
	REQUIRE(settings.getWString() == L"bar");
}

TEST_CASE("settings can be replaced when loaded")
{
	TestSettings settings;
	REQUIRE(settings.load(FilePath(L"data/SettingsTestSuite/settings.xml")));

	REQUIRE(settings.setBool(false));
	REQUIRE(settings.getBool() == false);

	REQUIRE(settings.setInt(2));
	REQUIRE(settings.getInt() == 2);

	REQUIRE(settings.setFloat(2.5f));
	REQUIRE(settings.getFloat() == 2.5f);

	REQUIRE(settings.setString("foo"));
	REQUIRE(settings.getString() == "foo");

	REQUIRE(settings.setWString(L"bar"));
	REQUIRE(settings.getWString() == L"bar");
}

TEST_CASE("settings can be added when loaded")
{
	TestSettings settings;
	REQUIRE(settings.load(FilePath(L"data/SettingsTestSuite/settings.xml")));

	REQUIRE(settings.getNewBool() == false);
	REQUIRE(settings.setNewBool(true));
	REQUIRE(settings.getNewBool() == true);
}

TEST_CASE("load project settings from file")
{
	ProjectSettings settings;
	REQUIRE(settings.load(FilePath(L"data/SettingsTestSuite/settings.xml")));
}

TEST_CASE("load source path from file")
{
	ProjectSettings projectSettings;
	projectSettings.load(FilePath(L"data/SettingsTestSuite/settings.xml"));
	std::shared_ptr<SourceGroupSettingsWithSourcePaths> sourceGroupSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(
			projectSettings.getAllSourceGroupSettings().front());
	std::vector<FilePath> paths = sourceGroupSettings->getSourcePaths();

	REQUIRE(paths.size() == 2);
	REQUIRE(paths[0].wstr() == L"src");
	REQUIRE(paths[1].wstr() == L"test");
}
