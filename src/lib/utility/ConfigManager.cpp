#include "ConfigManager.h"

#include <set>

#include "tinyxml.h"

#include "FilePath.h"
#include "TextAccess.h"
#include "logging.h"
#include "utility.h"
#include "utilityString.h"

std::shared_ptr<ConfigManager> ConfigManager::createEmpty()
{
	return std::shared_ptr<ConfigManager>(new ConfigManager());
}

std::shared_ptr<ConfigManager> ConfigManager::createAndLoad(const std::shared_ptr<TextAccess> textAccess)
{
	std::shared_ptr<ConfigManager> configManager = std::shared_ptr<ConfigManager>(new ConfigManager());
	configManager->load(textAccess);
	return configManager;
}

std::shared_ptr<ConfigManager> ConfigManager::createCopy()
{
	return std::shared_ptr<ConfigManager>(new ConfigManager(*this));
}

void ConfigManager::clear()
{
	m_values.clear();
}

bool ConfigManager::getValue(const std::string& key, std::string& value) const
{
	std::multimap<std::string, std::string>::const_iterator it = m_values.find(key);

	if (it != m_values.end())
	{
		value = it->second;
		return true;
	}
	else
	{
		if (m_warnOnEmptyKey)
		{
			// LOG_WARNING("value " + key + " is not present in config.");
		}
		return false;
	}
}

bool ConfigManager::getValue(const std::string& key, std::wstring& value) const
{
	std::string valueString;
	if (getValue(key, valueString))
	{
		value = utility::decodeFromUtf8(valueString.c_str());
		return true;
	}
	return false;
}

bool ConfigManager::getValue(const std::string& key, int& value) const
{
	std::string valueString;
	if (getValue(key, valueString))
	{
		value = atoi(valueString.c_str());
		return true;
	}
	return false;
}

bool ConfigManager::getValue(const std::string& key, float& value) const
{
	std::string valueString;
	if (getValue(key, valueString))
	{
		std::stringstream ss;
		ss << valueString;
		ss >> value;
		return true;
	}
	return false;
}

bool ConfigManager::getValue(const std::string& key, bool& value) const
{
	std::string valueString;
	if (getValue(key, valueString))
	{
		value = (atoi(valueString.c_str()) != 0);
		return true;
	}
	return false;
}

bool ConfigManager::getValue(const std::string& key, FilePath& value) const
{
	std::wstring valueString;
	if (getValue(key, valueString))
	{
		value = FilePath(valueString);
		return true;
	}
	return false;
}

bool ConfigManager::getValues(const std::string& key, std::vector<std::string>& values) const
{
	std::pair<
		std::multimap<std::string, std::string>::const_iterator,
		std::multimap<std::string, std::string>::const_iterator>
		ret;
	ret = m_values.equal_range(key);

	if (ret.first != ret.second)
	{
		for (std::multimap<std::string, std::string>::const_iterator cit = ret.first;
			 cit != ret.second;
			 ++cit)
		{
			values.push_back(cit->second);
		}
		return true;
	}
	else
	{
		if (m_warnOnEmptyKey)
		{
			// LOG_WARNING("value " + key + " is not present in config.");
		}
		return false;
	}
}

bool ConfigManager::getValues(const std::string& key, std::vector<std::wstring>& values) const
{
	std::vector<std::string> valuesStringVector;
	if (getValues(key, valuesStringVector))
	{
		for (const std::string& valueString: valuesStringVector)
		{
			values.push_back(utility::decodeFromUtf8(valueString));
		}
		return true;
	}
	return false;
}


bool ConfigManager::getValues(const std::string& key, std::vector<int>& values) const
{
	std::vector<std::string> valuesStringVector;
	if (getValues(key, valuesStringVector))
	{
		for (const std::string& valueString: valuesStringVector)
		{
			values.push_back(atoi(valueString.c_str()));
		}
		return true;
	}
	return false;
}

bool ConfigManager::getValues(const std::string& key, std::vector<float>& values) const
{
	std::vector<std::string> valuesStringVector;
	if (getValues(key, valuesStringVector))
	{
		for (const std::string& valueString: valuesStringVector)
		{
			values.push_back(static_cast<float>(atof(valueString.c_str())));
		}
		return true;
	}
	return false;
}

bool ConfigManager::getValues(const std::string& key, std::vector<bool>& values) const
{
	std::vector<std::string> valuesStringVector;
	if (getValues(key, valuesStringVector))
	{
		for (const std::string& valueString: valuesStringVector)
		{
			values.push_back(atoi(valueString.c_str()) != 0);
		}
		return true;
	}
	return false;
}

bool ConfigManager::getValues(const std::string& key, std::vector<FilePath>& values) const
{
	std::vector<std::wstring> valuesStringVector;
	if (getValues(key, valuesStringVector))
	{
		for (const std::wstring& valueString: valuesStringVector)
		{
			values.push_back(FilePath(valueString));
		}
		return true;
	}
	return false;
}

void ConfigManager::setValue(const std::string& key, const std::string& value)
{
	std::multimap<std::string, std::string>::iterator it = m_values.find(key);

	if (it != m_values.end())
	{
		it->second = value;
	}
	else
	{
		m_values.emplace(key, value);
	}
}

void ConfigManager::setValue(const std::string& key, const std::wstring& value)
{
	setValue(key, utility::encodeToUtf8(value));
}

void ConfigManager::setValue(const std::string& key, const int value)
{
	setValue(key, std::to_string(value));
}

void ConfigManager::setValue(const std::string& key, const float value)
{
	std::stringstream ss;
	ss << value;
	setValue(key, ss.str());
}

void ConfigManager::setValue(const std::string& key, const bool value)
{
	setValue(key, std::string(value ? "1" : "0"));
}

void ConfigManager::setValue(const std::string& key, const FilePath& value)
{
	setValue(key, value.wstr());
}

void ConfigManager::setValues(const std::string& key, const std::vector<std::string>& values)
{
	std::multimap<std::string, std::string>::iterator it = m_values.find(key);

	if (it != m_values.end())
	{
		m_values.erase(key);
	}
	for (std::string s: values)
	{
		m_values.emplace(key, s);
	}
}

void ConfigManager::setValues(const std::string& key, const std::vector<std::wstring>& values)
{
	std::vector<std::string> stringValues;
	for (std::wstring v: values)
	{
		stringValues.push_back(utility::encodeToUtf8(v));
	}
	setValues(key, stringValues);
}

void ConfigManager::setValues(const std::string& key, const std::vector<int>& values)
{
	std::vector<std::string> stringValues;
	for (int i: values)
	{
		stringValues.push_back(std::to_string(i));
	}
	setValues(key, stringValues);
}

void ConfigManager::setValues(const std::string& key, const std::vector<float>& values)
{
	std::vector<std::string> stringValues;
	for (float f: values)
	{
		stringValues.push_back(std::to_string(f));
	}
	setValues(key, stringValues);
}

void ConfigManager::setValues(const std::string& key, const std::vector<bool>& values)
{
	std::vector<std::string> stringValues;
	for (bool b: values)
	{
		stringValues.push_back(std::string(b ? "1" : "0"));
	}
	setValues(key, stringValues);
}

void ConfigManager::setValues(const std::string& key, const std::vector<FilePath>& values)
{
	std::vector<std::wstring> stringValues;
	for (const FilePath& p: values)
	{
		stringValues.push_back(p.wstr());
	}
	setValues(key, stringValues);
}

void ConfigManager::removeValues(const std::string& key)
{
	for (const std::string& sublevelKey: getSublevelKeys(key))
	{
		removeValues(sublevelKey);
	}
	m_values.erase(key);
}

bool ConfigManager::isValueDefined(const std::string& key) const
{
	std::multimap<std::string, std::string>::const_iterator it = m_values.find(key);

	return (it != m_values.end());
}

std::vector<std::string> ConfigManager::getSublevelKeys(const std::string& key) const
{
	std::set<std::string> keys;
	for (std::multimap<std::string, std::string>::const_iterator it = m_values.begin();
		 it != m_values.end();
		 it++)
	{
		if (utility::isPrefix(key, it->first))
		{
			size_t startPos = it->first.find("/", key.size());
			if (startPos == key.size())
			{
				std::string sublevelKey = it->first.substr(0, it->first.find("/", startPos + 1));
				keys.insert(sublevelKey);
			}
		}
	}
	return utility::toVector(keys);
}

bool ConfigManager::load(const std::shared_ptr<TextAccess> textAccess)
{
	TiXmlDocument doc;
	const char* pTest = doc.Parse(textAccess->getText().c_str(), 0, TIXML_ENCODING_UTF8);
	if (pTest != nullptr)
	{
		TiXmlHandle docHandle(&doc);
		TiXmlNode* rootNode = docHandle.FirstChild("config").ToNode();
		if (rootNode == nullptr)
		{
			LOG_ERROR("No rootelement 'config' in the configfile");
			return false;
		}
		for (TiXmlNode* childNode = rootNode->FirstChild(); childNode;
			 childNode = childNode->NextSibling())
		{
			parseSubtree(childNode, "");
		}
	}
	else
	{
		LOG_ERROR("Unable to load file.");
		return false;
	}
	return true;
}

bool ConfigManager::save(const std::string filepath)
{
	std::string output("");
	return createXmlDocument(true, filepath, output);
}

void ConfigManager::setWarnOnEmptyKey(bool warnOnEmptyKey) const
{
	m_warnOnEmptyKey = warnOnEmptyKey;
}

ConfigManager::ConfigManager(): m_warnOnEmptyKey(true) {}

ConfigManager::ConfigManager(const ConfigManager& other)
	: m_values(other.m_values), m_warnOnEmptyKey(other.m_warnOnEmptyKey)
{
}

bool ConfigManager::createXmlDocument(bool saveAsFile, const std::string filepath, std::string& output)
{
	bool success = true;
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
	doc.LinkEndChild(decl);
	TiXmlElement* root = new TiXmlElement("config");
	doc.LinkEndChild(root);

	for (std::multimap<std::string, std::string>::iterator it = m_values.begin();
		 it != m_values.end();
		 ++it)
	{
		if (!it->first.size() || !it->second.size())
		{
			continue;
		}

		std::vector<std::string> tokens = utility::splitToVector(it->first, "/");

		TiXmlElement* element = doc.RootElement();
		TiXmlElement* child;
		while (tokens.size() > 1)
		{
			child = element->FirstChildElement(tokens.front().c_str());
			if (!child)
			{
				child = new TiXmlElement(tokens.front().c_str());
				element->LinkEndChild(child);
			}
			tokens.erase(tokens.begin());
			element = child;
		}

		child = new TiXmlElement(tokens.front().c_str());
		element->LinkEndChild(child);
		TiXmlText* text = new TiXmlText(it->second.c_str());
		child->LinkEndChild(text);
	}

	if (saveAsFile)
	{
		success = doc.SaveFile(filepath.c_str());
	}
	else
	{
		TiXmlPrinter printer;
		doc.Accept(&printer);
		output = printer.CStr();
	}
	success = doc.SaveFile(filepath.c_str());
	doc.Clear();
	return success;
}

void ConfigManager::parseSubtree(TiXmlNode* currentNode, const std::string& currentPath)
{
	if (currentNode->Type() == TiXmlNode::TINYXML_TEXT)
	{
		std::string key = currentPath.substr(0, currentPath.size() - 1);
		m_values.insert(std::pair<std::string, std::string>(key, currentNode->ToText()->Value()));
	}
	else
	{
		for (TiXmlNode* childNode = currentNode->FirstChild(); childNode;
			 childNode = childNode->NextSibling())
		{
			parseSubtree(childNode, currentPath + std::string(currentNode->Value()) + "/");
		}
	}
}

std::string ConfigManager::toString()
{
	std::string output;
	createXmlDocument(false, "", output);
	return output;
}
