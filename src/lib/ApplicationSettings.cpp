#include "ApplicationSettings.h"

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

std::vector<std::string> ApplicationSettings::getHeaderSearchPaths() const
{
	//TODO: defaultValues?
	std::vector<std::string> defaultValues;
	return getValues("source/HeaderSearchPaths/HeaderSearchPath", defaultValues);
}

int ApplicationSettings::getCodeTabWidth() const
{
	return getValue<int>("code/TabWidth", 4);
}

void ApplicationSettings::setCodeTabWidth(int codeTabWidth)
{
	setValue<int>("code/TabWidth", codeTabWidth);
}

std::string ApplicationSettings::getCodeFontName() const
{
	return getValue<std::string>("code/FontName", "Courier");
}

void ApplicationSettings::setCodeFontName(const std::string& codeFontName)
{
	setValue<std::string>("code/FontName", codeFontName);
}

int ApplicationSettings::getCodeFontSize() const
{
	return getValue<int>("code/FontSize", 12);
}

void ApplicationSettings::setCodeFontSize(int codeFontSize)
{
	setValue<int>("code/FontSize", codeFontSize);
}

Colori ApplicationSettings::getCodeLinkColor() const
{
	return Colori::fromString(getValue<std::string>("code/LinkColor", Colori(255, 255, 0, 100).toString()));
}

void ApplicationSettings::setCodeLinkColor(Colori color)
{
	setValue<std::string>("code/LinkColor", color.toString());
}

Colori ApplicationSettings::getCodeScopeColor() const
{
	return Colori::fromString(getValue<std::string>("code/ScopeColor", Colori(255, 255, 0, 100).toString()));
}

void ApplicationSettings::setCodeScopeColor(Colori color)
{
	setValue<std::string>("code/ScopeColor", color.toString());
}

Colori ApplicationSettings::getCodeActiveLinkColor() const
{
	return Colori::fromString(getValue<std::string>("code/ActiveLinkColor", Colori(0, 255, 0, 100).toString()));
}

void ApplicationSettings::setCodeActiveLinkColor(Colori color)
{
	setValue<std::string>("code/ActiveLinkColor", color.toString());
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
