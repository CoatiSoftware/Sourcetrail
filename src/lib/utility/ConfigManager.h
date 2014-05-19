#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H


#include <map>
#include <memory>
#include <string>

#include "tinyxml/tinyxml.h"

class TextAccess;

class ConfigManager
{
public:
	static std::shared_ptr<ConfigManager> createAndLoad(const std::shared_ptr<TextAccess> textAccess);

	bool getValue(const std::string& key, std::string& value) const;
	bool getValue(const std::string& key, int& value) const;
	bool getValue(const std::string& key, float& value) const;
	bool getValue(const std::string& key, bool& value) const;

	void setValue(const std::string& key, const std::string& value);
	void setValue(const std::string& key, const int value);
	void setValue(const std::string& key, const float value);
	void setValue(const std::string& key, const bool value);

	void load();
	void save();

private:
	ConfigManager(const std::shared_ptr<TextAccess> textAccess);
	ConfigManager(const ConfigManager&);
	ConfigManager operator=(const ConfigManager&);

	void parseSubtree(TiXmlNode* parentElement, const std::string& currentPath);

	const std::shared_ptr<TextAccess> m_textAccess;
	std::map<std::string, std::string> m_values;
};


#endif // CONFIG_MANAGER_H
