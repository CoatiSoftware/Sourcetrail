#ifndef SOLUTION_PARSER_UTILITY_H
#define SOLUTION_PARSER_UTILITY_H

#include <string>
#include <vector>

#include "tinyxml/tinyxml.h"

class SolutionParserUtility
{
public:
	static TiXmlElement* getFirstTagByName(TiXmlElement* root, const std::string& tag);
	static std::vector<TiXmlElement*> getAllTagsByName(TiXmlElement* root, const std::string& tag);

	static TiXmlElement* getFirstTagByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute);
	static std::vector<TiXmlElement*> getAllTagsByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute);

	static bool checkValidFileExtension(const std::string& file, const std::vector<std::string>& validExtensions);

	static std::vector<std::string> resolveEnvironmentVariables(const std::vector<std::string>& paths);
	static std::string findAndResolveEnvironmentVariable(const std::string& path);
	static std::vector<std::string> makePathsCanonical(const std::vector<std::string>& paths);
};

#endif // SOLUTION_PARSER_UTILITY_H
