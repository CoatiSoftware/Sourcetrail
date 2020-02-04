#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

class TextAccess;
class TiXmlNode;
class FilePath;

class ConfigManager
{
public:
	static std::shared_ptr<ConfigManager> createEmpty();
	static std::shared_ptr<ConfigManager> createAndLoad(const std::shared_ptr<TextAccess> textAccess);
	std::shared_ptr<ConfigManager> createCopy();

	void clear();

	bool getValue(const std::string& key, std::string& value) const;
	bool getValue(const std::string& key, std::wstring& value) const;
	bool getValue(const std::string& key, int& value) const;
	bool getValue(const std::string& key, float& value) const;
	bool getValue(const std::string& key, bool& value) const;
	bool getValue(const std::string& key, FilePath& value) const;

	template <typename T>
	T getValueOrDefault(const std::string& key, T defaultValue) const;

	bool getValues(const std::string& key, std::vector<std::string>& values) const;
	bool getValues(const std::string& key, std::vector<std::wstring>& values) const;
	bool getValues(const std::string& key, std::vector<int>& values) const;
	bool getValues(const std::string& key, std::vector<float>& values) const;
	bool getValues(const std::string& key, std::vector<bool>& values) const;
	bool getValues(const std::string& key, std::vector<FilePath>& values) const;

	template <typename T>
	std::vector<T> getValuesOrDefaults(const std::string& key, std::vector<T> defaultValues) const;

	void setValue(const std::string& key, const std::string& value);
	void setValue(const std::string& key, const std::wstring& value);
	void setValue(const std::string& key, const int value);
	void setValue(const std::string& key, const float value);
	void setValue(const std::string& key, const bool value);
	void setValue(const std::string& key, const FilePath& value);

	void setValues(const std::string& key, const std::vector<std::string>& values);
	void setValues(const std::string& key, const std::vector<std::wstring>& values);
	void setValues(const std::string& key, const std::vector<int>& values);
	void setValues(const std::string& key, const std::vector<float>& values);
	void setValues(const std::string& key, const std::vector<bool>& values);
	void setValues(const std::string& key, const std::vector<FilePath>& values);

	void removeValues(const std::string& key);

	bool isValueDefined(const std::string& key) const;
	std::vector<std::string> getSublevelKeys(const std::string& key) const;

	bool load(const std::shared_ptr<TextAccess> textAccess);
	bool save(const std::string filepath);
	std::string toString();

	void setWarnOnEmptyKey(bool warnOnEmptyKey) const;

private:
	ConfigManager();
	ConfigManager(const ConfigManager&);
	void operator=(const ConfigManager&) = delete;

	void parseSubtree(TiXmlNode* parentElement, const std::string& currentPath);
	bool createXmlDocument(bool saveAsFile, std::string filepath, std::string& output);

	std::multimap<std::string, std::string> m_values;
	mutable bool m_warnOnEmptyKey;
};

template <typename T>
T ConfigManager::getValueOrDefault(const std::string& key, T defaultValue) const
{
	T value;
	if (getValue(key, value))
	{
		return value;
	}
	return defaultValue;
}

template <typename T>
std::vector<T> ConfigManager::getValuesOrDefaults(const std::string& key, std::vector<T> defaultValues) const
{
	std::vector<T> values;
	if (getValues(key, values))
	{
		return values;
	}
	return defaultValues;
}

#endif	  // CONFIG_MANAGER_H
