#include <cxxtest/TestSuite.h>

#include "settings/Settings.h"
#include "settings/SettingsMigrator.h"

class SettingsMigratorTestSuite : public CxxTest::TestSuite
{
public:
	void test_migrator_changes_nothing_without_migrations_except_version()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_name()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "value", "int");
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_path()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <sub>\n"
			"        <int>2</int>\n"
			"    </sub>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "value", "sub/int");
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_group_name()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <values>\n"
			"        <value>2</value>\n"
			"        <value>3</value>\n"
			"        <value>4</value>\n"
			"    </values>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <vals>\n"
			"        <value>2</value>\n"
			"        <value>3</value>\n"
			"        <value>4</value>\n"
			"    </vals>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "values/value", "vals/value");
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_group_element_name()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <values>\n"
			"        <value>2</value>\n"
			"        <value>3</value>\n"
			"        <value>4</value>\n"
			"    </values>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <values>\n"
			"        <val>2</val>\n"
			"        <val>3</val>\n"
			"        <val>4</val>\n"
			"    </values>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "values/value", "values/val");
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_only_up_specified_version()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "value", "int");
		migrator.addMigration(2, "int", "val");
		migrator.migrate(&settingsBefore, 1);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_only_from_specified_version()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <version>1</version>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <val>2</val>\n"
			"    <version>2</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "value", "int");
		migrator.addMigration(2, "int", "val");
		migrator.migrate(&settingsBefore, 2);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_for_multiple_versions()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <val>2</val>\n"
			"    <version>2</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "value", "int");
		migrator.addMigration(2, "int", "val");
		migrator.migrate(&settingsBefore, 2);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_for_multiple_migrations()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"    <element>hi there</element>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <val>2</val>\n"
			"    <ele>hi there</ele>\n"
			"    <version>2</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "value", "val");
		migrator.addMigration(1, "element", "ele");
		migrator.migrate(&settingsBefore, 2);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

	void test_migrator_changes_for_multiple_versions_and_migrations()
	{
		Settings settingsBefore = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <value>2</value>\n"
			"    <element>two</element>\n"
			"    <element>three</element>\n"
			"    <element>four</element>\n"
			"</config>\n"
		);

		Settings settingsAfter = createSettings(
			"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
			"<config>\n"
			"    <int>2</int>\n"
			"    <elements>\n"
			"        <element>two</element>\n"
			"        <element>three</element>\n"
			"        <element>four</element>\n"
			"    </elements>\n"
			"    <version>3</version>\n"
			"</config>\n"
		);

		SettingsMigrator migrator;
		migrator.addMigration(1, "value", "int/val");
		migrator.addMigration(1, "element", "ele");
		migrator.addMigration(2, "int/val", "int");
		migrator.addMigration(3, "ele", "elements/element");
		migrator.migrate(&settingsBefore, 3);

		TS_ASSERT_EQUALS(settingsBefore.getAsText(), settingsAfter.getAsText());
	}

private:
	Settings createSettings(const std::string& text)
	{
		return Settings::createFromText(TextAccess::createFromString(text));
	}
};
