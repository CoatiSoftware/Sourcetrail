#include "settings/ApplicationSettings.h"

std::shared_ptr<ApplicationSettings> ApplicationSettings::s_instance;

std::shared_ptr<ApplicationSettings> ApplicationSettings::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ApplicationSettings>(new ApplicationSettings());
	}

	return s_instance;
}

ApplicationSettings::~ApplicationSettings()
{
}

std::string ApplicationSettings::getStartupProjectFilePath() const
{
	return getValue<std::string>("StartupProject", "");
}

bool ApplicationSettings::filterUndefinedNodesFromGraph() const
{
	return getValue<bool>("FilterUndefinedNodesFromGraph", false);
}

std::string ApplicationSettings::getFontName() const
{
	return getValue<std::string>("application/font_name", "Source Code Pro");
}

void ApplicationSettings::setFontName(const std::string& fontName)
{
	setValue<std::string>("application/font_name", fontName);
}

int ApplicationSettings::getFontSize() const
{
	return getValue<int>("application/font_size", 14);
}

void ApplicationSettings::setFontSize(int fontSize)
{
	setValue<int>("application/font_size", fontSize);
}

int ApplicationSettings::getCodeTabWidth() const
{
	return getValue<int>("code/TabWidth", 4);
}

void ApplicationSettings::setCodeTabWidth(int codeTabWidth)
{
	setValue<int>("code/TabWidth", codeTabWidth);
}

int ApplicationSettings::getCodeSnippetSnapRange() const
{
	return getValue<int>("code/snippet/snap_range", 4);
}

void ApplicationSettings::setCodeSnippetSnapRange(int range)
{
	setValue<int>("code/snippet/snap_range", range);
}

int ApplicationSettings::getCodeSnippetExpandRange() const
{
	return getValue<int>("code/snippet/expand_range", 2);
}

void ApplicationSettings::setCodeSnippetExpandRange(int range)
{
	setValue<int>("code/snippet/expand_range", range);
}

std::string ApplicationSettings::getNodeTypeColor(Node::NodeType type, const std::string& state) const
{
	std::string path = "colors/" + Node::getTypeString(type) + "/" + state;
	return getValue<std::string>(path, "#FFFFFF");
}

void ApplicationSettings::setNodeTypeColor(Node::NodeType type, const std::string& color, const std::string& state)
{
	std::string path = "colors/" + Node::getTypeString(type) + "/" + state;
	setValue<std::string>(path, color);
}

std::string ApplicationSettings::getQueryNodeTypeColor(QueryNode::QueryNodeType type, const std::string& state) const
{
	std::string path = "colors/" + QueryNode::queryNodeTypeToString(type) + "/" + state;
	return getValue<std::string>(path, "#FFFFFF");
}

void ApplicationSettings::setQueryNodeTypeColor(QueryNode::QueryNodeType type,
	const std::string& color, const std::string& state)
{
	std::string path = "colors/" + QueryNode::queryNodeTypeToString(type) + "/" + state;
	setValue<std::string>(path, color);
}

ApplicationSettings::ApplicationSettings()
{
}
