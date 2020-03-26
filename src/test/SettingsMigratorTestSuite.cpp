#include "catch.hpp"

#include "Settings.h"
#include "SettingsMigrationLambda.h"
#include "SettingsMigrationMoveKey.h"
#include "SettingsMigrator.h"
#include "TextAccess.h"

namespace
{
class MigratorTestSettings: public Settings
{
public:
	static MigratorTestSettings createFromText(const std::shared_ptr<TextAccess> textAccess)
	{
		MigratorTestSettings settings;
		settings.m_config = ConfigManager::createAndLoad(textAccess);
		return settings;
	}

	std::string getAsText() const
	{
		if (m_config)
		{
			return m_config->toString();
		}

		return "";
	}
};

MigratorTestSettings createSettings(const std::string& text)
{
	return MigratorTestSettings::createFromText(TextAccess::createFromString(text));
}
}	 // namespace

TEST_CASE("migrator changes nothing without migrations except version")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"    <version>1</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.migrate(&settingsBefore, 1);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes name")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <int>2</int>\n"
		"    <version>1</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
	migrator.migrate(&settingsBefore, 1);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes path")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <sub>\n"
		"        <int>2</int>\n"
		"    </sub>\n"
		"    <version>1</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "sub/int"));
	migrator.migrate(&settingsBefore, 1);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes group name")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <values>\n"
		"        <value>2</value>\n"
		"        <value>3</value>\n"
		"        <value>4</value>\n"
		"    </values>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <vals>\n"
		"        <value>2</value>\n"
		"        <value>3</value>\n"
		"        <value>4</value>\n"
		"    </vals>\n"
		"    <version>1</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(
		1, std::make_shared<SettingsMigrationMoveKey>("values/value", "vals/value"));
	migrator.migrate(&settingsBefore, 1);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes group element name")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <values>\n"
		"        <value>2</value>\n"
		"        <value>3</value>\n"
		"        <value>4</value>\n"
		"    </values>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <values>\n"
		"        <val>2</val>\n"
		"        <val>3</val>\n"
		"        <val>4</val>\n"
		"    </values>\n"
		"    <version>1</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(
		1, std::make_shared<SettingsMigrationMoveKey>("values/value", "values/val"));
	migrator.migrate(&settingsBefore, 1);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes only up specified version")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <int>2</int>\n"
		"    <version>1</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int", "val"));
	migrator.migrate(&settingsBefore, 1);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes only from specified version")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <int>2</int>\n"
		"    <version>1</version>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <val>2</val>\n"
		"    <version>2</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int", "val"));
	migrator.migrate(&settingsBefore, 2);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes for multiple versions")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <val>2</val>\n"
		"    <version>2</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int", "val"));
	migrator.migrate(&settingsBefore, 2);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes for multiple migrations")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"    <element>hi there</element>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <val>2</val>\n"
		"    <ele>hi there</ele>\n"
		"    <version>2</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "val"));
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("element", "ele"));
	migrator.migrate(&settingsBefore, 2);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator changes for multiple versions and migrations")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"    <element>two</element>\n"
		"    <element>three</element>\n"
		"    <element>four</element>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <int>2</int>\n"
		"    <elements>\n"
		"        <element>two</element>\n"
		"        <element>three</element>\n"
		"        <element>four</element>\n"
		"    </elements>\n"
		"    <version>3</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("value", "int/val"));
	migrator.addMigration(1, std::make_shared<SettingsMigrationMoveKey>("element", "ele"));
	migrator.addMigration(2, std::make_shared<SettingsMigrationMoveKey>("int/val", "int"));
	migrator.addMigration(3, std::make_shared<SettingsMigrationMoveKey>("ele", "elements/element"));
	migrator.migrate(&settingsBefore, 3);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator with lambda")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>4</value>\n"
		"    <version>1</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(
		1,
		std::make_shared<SettingsMigrationLambda>([](const SettingsMigration* migration,
													 Settings* settings) {
			migration->setValueInSettings<int>(
				settings, "value", migration->getValueFromSettings<int>(settings, "value", 0) * 2);
		}));
	migrator.migrate(&settingsBefore, 1);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}

TEST_CASE("migrator with multiple lambdas")
{
	MigratorTestSettings settingsBefore = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>2</value>\n"
		"</config>\n");

	MigratorTestSettings settingsAfter = createSettings(
		"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
		"<config>\n"
		"    <value>3</value>\n"
		"    <version>2</version>\n"
		"</config>\n");

	SettingsMigrator migrator;
	migrator.addMigration(
		1,
		std::make_shared<SettingsMigrationLambda>([](const SettingsMigration* migration,
													 Settings* settings) {
			migration->setValueInSettings<int>(
				settings, "value", migration->getValueFromSettings<int>(settings, "value", 0) * 2);
		}));
	migrator.addMigration(
		2,
		std::make_shared<SettingsMigrationLambda>([](const SettingsMigration* migration,
													 Settings* settings) {
			migration->setValueInSettings<int>(
				settings, "value", migration->getValueFromSettings<int>(settings, "value", 0) - 1);
		}));
	migrator.migrate(&settingsBefore, 2);

	REQUIRE(settingsBefore.getAsText() == settingsAfter.getAsText());
}
