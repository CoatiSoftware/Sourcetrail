#ifndef SOLUTION_PARSER_VISUAL_STUDIO_H
#define SOLUTION_PARSER_VISUAL_STUDIO_H

#include "ISolutionParser.h"

#include "tinyxml/tinyxml.h"

class SolutionParserVisualStudio : public ISolutionParser
{
public:
	SolutionParserVisualStudio();
	virtual ~SolutionParserVisualStudio();

	virtual std::string getSolutionName();

	virtual std::vector<std::string> getProjects();
	virtual std::vector<std::string> getProjectFiles();
	virtual std::vector<std::string> getProjectItems();
	virtual std::vector<std::string> getIncludePaths();

	std::vector<std::string> getProjectItemsNonCanonical(); // for testing purposes, paths returned are non canonical, don't use if you don't know what that means
	std::vector<std::string> getIncludePathsNonCanonical(); // for testing purposes, paths returned are non canonical, don't use if you don't know what that means

private:
	std::vector<std::string> findProjectItems();
	std::vector<std::string> findIncludePaths();

	bool checkValidFileExtension(const std::string& file, const std::vector<std::string>& validExtensions);

	std::vector<std::string> getProjectBlocks(const std::string& solution) const;
	std::string getProjectFilePath(const std::string& projectBlock) const;

	std::vector<std::string> seperateIncludePaths(const std::vector<std::string>& includePaths) const;
	std::vector<std::string> seperateIncludePaths(const std::string& includePaths) const;

	TiXmlElement* getFirstTagByName(TiXmlElement* root, const std::string& tag);
	std::vector<TiXmlElement*> getAllTagsByName(TiXmlElement* root, const std::string& tag);

	TiXmlElement* getFirstTagByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute);
	std::vector<TiXmlElement*> getAllTagsByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute);

	std::vector<std::string> resolveEnvironmentVariables(const std::vector<std::string>& paths);
	std::string findAndResolveEnvironmentVariable(const std::string& path);
	std::vector<std::string> makePathsAbsolute(const std::vector<std::string>& paths);
	std::vector<std::string> makePathsCanonical(const std::vector<std::string>& paths);
};

#endif // SOLUTION_PARSER_VISUAL_STUDIO_H
