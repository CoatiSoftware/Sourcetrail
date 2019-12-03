#include "ColorScheme.h"

#include "utilityString.h"

std::shared_ptr<ColorScheme> ColorScheme::s_instance;

std::shared_ptr<ColorScheme> ColorScheme::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ColorScheme>(new ColorScheme());
	}

	return s_instance;
}

ColorScheme::~ColorScheme() {}

bool ColorScheme::hasColor(const std::string& key) const
{
	return isValueDefined(key);
}

std::string ColorScheme::getColor(const std::string& key) const
{
	return getValue<std::string>(key, "#FF1493");
}

std::string ColorScheme::getColor(const std::string& key, const std::string& defaultColor) const
{
	return getValue<std::string>(key, defaultColor);
}

std::string ColorScheme::getNodeTypeColor(NodeType type, const std::string& key, ColorState state) const
{
	return getNodeTypeColor(type.getUnderscoredTypeString(), key, state);
}

std::string ColorScheme::getNodeTypeColor(
	const std::string& typeStr, const std::string& key, ColorState state) const
{
	disableWarnings();

	const std::string type = getValue<std::string>("graph/node/" + typeStr + "/like", typeStr);
	std::string color = getValue<std::string>(
		"graph/node/" + type + "/" + key + "/" + stateToString(state), "");

	if (!color.size() && state != NORMAL)
	{
		color = getValue<std::string>(
			"graph/node/" + type + "/" + key + "/" + stateToString(NORMAL), "");
	}

	if (!color.size())
	{
		color = getValue<std::string>("graph/node/default/" + key + "/" + stateToString(state), "");
	}

	enableWarnings();

	if (!color.size() && state != NORMAL)
	{
		color = getValue<std::string>(
			"graph/node/default/" + key + "/" + stateToString(NORMAL), "#FFFFFF");
	}

	return color;
}

std::string ColorScheme::getEdgeTypeColor(Edge::EdgeType type, ColorState state) const
{
	return getEdgeTypeColor(utility::encodeToUtf8(Edge::getUnderscoredTypeString(type)), state);
}

std::string ColorScheme::getEdgeTypeColor(const std::string& typeStr, ColorState state) const
{
	disableWarnings();

	std::string type = getValue<std::string>("graph/edge/" + typeStr + "/like", typeStr);
	std::string color = getValue<std::string>("graph/edge/" + type + "/" + stateToString(state), "");

	if (!color.size() && state != NORMAL)
	{
		color = getValue<std::string>("graph/edge/" + type + "/" + stateToString(NORMAL), "");
	}

	if (!color.size())
	{
		color = getValue<std::string>("graph/edge/default/" + stateToString(state), "");
	}

	enableWarnings();

	if (!color.size() && state != NORMAL)
	{
		color = getValue<std::string>("graph/edge/default/" + stateToString(NORMAL), "#FFFFFF");
	}

	return color;
}

std::string ColorScheme::getSearchTypeColor(
	const std::string& searchTypeName, const std::string& key, const std::string& state) const
{
	std::string path = "search/query/" + searchTypeName + "/" + state + "/" + key;
	return getValue<std::string>(path, "#FFFFFF");
}

std::string ColorScheme::getSyntaxColor(const std::string& key) const
{
	return getValue<std::string>("code/snippet/syntax/" + key, "#FFFFFF");
}

std::string ColorScheme::getCodeAnnotationTypeColor(
	const std::string& typeStr, const std::string& key, ColorState state) const
{
	disableWarnings();
	std::string color = getValue<std::string>(
		"code/snippet/annotation/" + typeStr + "/" + stateToString(state) + "/" + key, "");

	if (!color.size() && state == ACTIVE)
	{
		color = getValue<std::string>(
			"code/snippet/annotation/" + typeStr + "/" + stateToString(FOCUS) + "/" + key, "");
	}

	if (!color.size() && state != NORMAL)
	{
		color = getValue<std::string>(
			"code/snippet/annotation/" + typeStr + "/" + stateToString(NORMAL) + "/" + key, "");
	}

	if (!color.size())
	{
		color = "transparent";
	}

	enableWarnings();

	return color;
}

ColorScheme::ColorScheme() {}

std::string ColorScheme::stateToString(ColorState state)
{
	switch (state)
	{
	case NORMAL:
		return "normal";
	case FOCUS:
		return "focus";
	case ACTIVE:
		return "active";
	}

	return "";
}
