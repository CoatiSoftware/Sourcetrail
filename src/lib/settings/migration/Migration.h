#ifndef MIGRATION_H
#define MIGRATION_H

#include <string>
#include <vector>

#include "settings/Settings.h"

class Migration
{
public:
	virtual ~Migration();
	virtual void apply(Settings* settings) const = 0;

	bool isValueDefinedInSettings(const Settings* settings, const std::string& key) const;

	template <typename T>
	T getValueFromSettings(Settings* settings, const std::string& key, T defaultValue) const;

	template<typename T>
	std::vector<T> getValuesFromSettings(Settings* settings, const std::string& key, std::vector<T> defaultValues) const;

	template<typename T>
	bool setValueInSettings(Settings* settings, const std::string& key, T value) const;

	template<typename T>
	bool setValuesInSettings(Settings* settings, const std::string& key, std::vector<T> values) const;

	void removeValuesInSettings(Settings* settings, const std::string& key) const;
};

template <typename T>
T Migration::getValueFromSettings(Settings* settings, const std::string& key, T defaultValue) const
{
	return settings->getValue(key, defaultValue);
}

template <typename T>
std::vector<T> Migration::getValuesFromSettings(Settings* settings, const std::string& key, std::vector<T> defaultValues) const
{
	return settings->getValues(key, defaultValues);
}

template <typename T>
bool Migration::setValueInSettings(Settings* settings, const std::string& key, T value) const
{
	return settings->setValue(key, value);
}

template <typename T>
bool Migration::setValuesInSettings(Settings* settings, const std::string& key, std::vector<T> values) const
{
	return settings->setValues(key, values);
}

#endif // MIGRATION_H
