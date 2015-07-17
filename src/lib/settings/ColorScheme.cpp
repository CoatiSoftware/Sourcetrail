#include "settings/ColorScheme.h"

std::shared_ptr<ColorScheme> ColorScheme::s_instance;

std::shared_ptr<ColorScheme> ColorScheme::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ColorScheme>(new ColorScheme());
	}

	return s_instance;
}

ColorScheme::~ColorScheme()
{
}

std::string ColorScheme::getColor(const std::string& key) const
{
	return getValue<std::string>(key, "#FFFFFF");
}

std::string ColorScheme::getNodeTypeColor(Node::NodeType type, const std::string& state) const
{
	std::string path = "graph/node/" + Node::getTypeString(type) + "/" + state;
	return getValue<std::string>(path, "#FFFFFF");
}

std::string ColorScheme::getEdgeTypeColor(Edge::EdgeType type, const std::string& state) const
{
	std::string path = "graph/edge/" + Edge::getTypeString(type) + "/" + state;
	return getValue<std::string>(path, "#FFFFFF");
}

std::string ColorScheme::getQueryNodeTypeColor(QueryNode::QueryNodeType type, const std::string& state) const
{
	std::string path = "search/query/" + QueryNode::queryNodeTypeToString(type) + "/" + state;
	return getValue<std::string>(path, "#FFFFFF");
}

std::string ColorScheme::getSyntaxColor(const std::string& key) const
{
	return getValue<std::string>("code/snippet/syntax/" + key, "#FFFFFF");
}

ColorScheme::ColorScheme()
{
}
