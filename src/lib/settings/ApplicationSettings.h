#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <memory>

#include "utility/math/Color.h"

#include "data/graph/Node.h"
#include "data/query/QueryNode.h"
#include "settings/CommonSettings.h"

class ApplicationSettings
	: public CommonSettings
{
public:
	static std::shared_ptr<ApplicationSettings> getInstance();
	~ApplicationSettings();

	std::string getStartupProjectFilePath() const;
	bool filterUndefinedNodesFromGraph() const;

	// application
	std::string getFontName() const;
	void setFontName(const std::string& fontName);

	int getFontSize() const;
	void setFontSize(int fontSize);

	// code
	int getCodeTabWidth() const;
	void setCodeTabWidth(int codeTabWidth);

	int getCodeSnippetSnapRange() const;
	void setCodeSnippetSnapRange(int range);

	int getCodeSnippetExpandRange() const;
	void setCodeSnippetExpandRange(int range);

	// colors
	std::string getNodeTypeColor(Node::NodeType type, const std::string& state = "normal") const;
	void setNodeTypeColor(Node::NodeType type, const std::string& color, const std::string& state = "normal");

	std::string getQueryNodeTypeColor(QueryNode::QueryNodeType type , const std::string& state = "normal") const;
	void setQueryNodeTypeColor(QueryNode::QueryNodeType type, const std::string& color, const std::string& state = "normal");

private:
	ApplicationSettings();
	ApplicationSettings(const ApplicationSettings&);
	void operator=(const ApplicationSettings&);

	static std::shared_ptr<ApplicationSettings> s_instance;
};

#endif // APPLICATION_SETTINGS_H
