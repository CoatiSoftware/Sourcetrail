#ifndef SETTINGS_MIGRATION_H
#define SETTINGS_MIGRATION_H

#include <string>
#include <vector>

#include "../../utility/migration/Migration.h"
#include "../Settings.h"

class SettingsMigration: public Migration<Settings>
{
public:
	virtual ~SettingsMigration();

	bool isValueDefinedInSettings(const Settings* settings, const std::string& key) const;

	template <typename T>
	T getValueFromSettings(Settings* settings, const std::string& key, T defaultValue) const;

	template <typename T>
	std::vector<T> getValuesFromSettings(
		Settings* settings, const std::string& key, std::vector<T> defaultValues) const;

	template <typename T>
	bool setValueInSettings(Settings* settings, const std::string& key, T value) const;

	template <typename T>
	bool setValuesInSettings(Settings* settings, const std::string& key, std::vector<T> values) const;

	void removeValuesInSettings(Settings* settings, const std::string& key) const;
};

template <typename T>
T SettingsMigration::getValueFromSettings(Settings* settings, const std::string& key, T defaultValue) const
{
	return settings->getValue(key, defaultValue);
}

template <typename T>
std::vector<T> SettingsMigration::getValuesFromSettings(
	Settings* settings, const std::string& key, std::vector<T> defaultValues) const
{
	return settings->getValues(key, defaultValues);
}

template <typename T>
bool SettingsMigration::setValueInSettings(Settings* settings, const std::string& key, T value) const
{
	return settings->setValue(key, value);
}

template <typename T>
bool SettingsMigration::setValuesInSettings(
	Settings* settings, const std::string& key, std::vector<T> values) const
{
	return settings->setValues(key, values);
}

#endif	  // SETTINGS_MIGRATION_H
