#include "component/view/GraphViewStyle.h"

#include "utility/logging/logging.h"

#include "component/view/GraphViewStyleImpl.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

GraphViewStyle::NodeMargins::NodeMargins()
	: left(0)
	, right(0)
	, top(0)
	, bottom(0)
	, spacingX(0)
	, spacingY(0)
	, minWidth(0)
	, charWidth(0.0f)
	, charHeight(0.0f)
	, iconWidth(0)
{
}

GraphViewStyle::NodeStyle::NodeStyle()
	: shadowBlurRadius(0)
	, cornerRadius(0)
	, borderWidth(0)
	, borderDashed(false)
	, fontSize(0)
	, fontBold(false)
	, iconSize(0)
{
}

GraphViewStyle::EdgeStyle::EdgeStyle()
	: width(0)
	, zValue(0)
	, isStraight(false)
	, arrowLength(0)
	, arrowWidth(0)
	, arrowClosed(false)
	, cornerRadius(0)
	, verticalOffset(0)
{
}

std::shared_ptr<GraphViewStyleImpl> GraphViewStyle::getImpl()
{
	if (!s_impl)
	{
		LOG_ERROR("No GraphViewStyleImpl instance set.");
		return nullptr;
	}

	return s_impl;
}

void GraphViewStyle::setImpl(std::shared_ptr<GraphViewStyleImpl> impl)
{
	s_impl = impl;
}

void GraphViewStyle::loadStyleSettings()
{
	s_fontSize = ApplicationSettings::getInstance()->getFontSize();
	s_fontName = ApplicationSettings::getInstance()->getFontName();

	s_charWidths.clear();
	s_charHeights.clear();
}

float GraphViewStyle::getCharWidthForNodeType(Node::NodeType type)
{
	std::map<Node::NodeType, float>::const_iterator it = s_charWidths.find(type);

	if (it != s_charWidths.end())
	{
		return it->second;
	}

	float charWidth = getImpl()->getCharWidthForNodeType(type);
	s_charWidths.emplace(type, charWidth);
	return charWidth;
}

float GraphViewStyle::getCharHeightForNodeType(Node::NodeType type)
{
	std::map<Node::NodeType, float>::const_iterator it = s_charHeights.find(type);

	if (it != s_charHeights.end())
	{
		return it->second;
	}

	float charHeight = getImpl()->getCharHeightForNodeType(type);
	s_charHeights.emplace(type, charHeight);
	return charHeight;
}

size_t GraphViewStyle::getFontSizeForNodeType(Node::NodeType type)
{
	switch (type)
	{
	case Node::NODE_UNDEFINED:
	case Node::NODE_NAMESPACE:
		return s_fontSize - 2;

	case Node::NODE_UNDEFINED_TYPE:
	case Node::NODE_STRUCT:
	case Node::NODE_CLASS:
	case Node::NODE_ENUM:
	case Node::NODE_TYPEDEF:
	case Node::NODE_TEMPLATE_PARAMETER_TYPE:
	case Node::NODE_FILE:
		return s_fontSize;

	case Node::NODE_UNDEFINED_FUNCTION:
	case Node::NODE_UNDEFINED_VARIABLE:
	case Node::NODE_FUNCTION:
	case Node::NODE_METHOD:
	case Node::NODE_GLOBAL_VARIABLE:
	case Node::NODE_FIELD:
	case Node::NODE_ENUM_CONSTANT:
		return s_fontSize - 3;
	}
}

size_t GraphViewStyle::getFontSizeOfAccessNode()
{
	return s_fontSize - 3;
}

size_t GraphViewStyle::getFontSizeOfExpandToggleNode()
{
	return s_fontSize - 5;
}

size_t GraphViewStyle::getFontSizeOfCountCircle()
{
	return s_fontSize - 3;
}

std::string GraphViewStyle::getFontNameForNodeType(Node::NodeType type)
{
	return s_fontName;
}

std::string GraphViewStyle::getFontNameOfAccessNode()
{
	return "Myriad Pro";
}

std::string GraphViewStyle::getFontNameOfExpandToggleNode()
{
	return "Myriad Pro";
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsForNodeType(Node::NodeType type, bool hasChildren)
{
	NodeMargins margins;
	margins.spacingX = 6;
	margins.spacingY = 8;

	switch (type)
	{
	case Node::NODE_UNDEFINED:
	case Node::NODE_NAMESPACE:
		margins.left = margins.right = 15;
		margins.top = 10;
		margins.bottom = 15;
		break;

	case Node::NODE_FILE:
		margins.iconWidth = s_fontSize + 11;
	case Node::NODE_UNDEFINED_TYPE:
	case Node::NODE_STRUCT:
	case Node::NODE_CLASS:
	case Node::NODE_ENUM:
	case Node::NODE_TYPEDEF:
	case Node::NODE_TEMPLATE_PARAMETER_TYPE:
		if (hasChildren)
		{
			margins.left = margins.right = 10;
			margins.top = 15;
			margins.bottom = 10;
		}
		else
		{
			margins.left = margins.right = 8;
			margins.top = margins.bottom = 8;
		}
		break;

	case Node::NODE_UNDEFINED_FUNCTION:
	case Node::NODE_FUNCTION:
	case Node::NODE_METHOD:
	case Node::NODE_UNDEFINED_VARIABLE:
	case Node::NODE_GLOBAL_VARIABLE:
	case Node::NODE_FIELD:
	case Node::NODE_ENUM_CONSTANT:
		margins.left = margins.right = 5;
		margins.top = margins.bottom = 3;
		break;
	}

	margins.charWidth = getCharWidthForNodeType(type);
	margins.charHeight = getCharHeightForNodeType(type);

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfAccessNode(TokenComponentAccess::AccessType type)
{
	NodeMargins margins;
	margins.spacingX = margins.spacingY = 8;

	margins.left = margins.right = 10;
	margins.top = 40;
	margins.bottom = 10;

	switch (type)
	{
	case TokenComponentAccess::ACCESS_NONE:
		margins.minWidth = 30;
		break;
	case TokenComponentAccess::ACCESS_PUBLIC:
		margins.minWidth = 56;
		break;
	case TokenComponentAccess::ACCESS_PROTECTED:
		margins.minWidth = 80;
		break;
	case TokenComponentAccess::ACCESS_PRIVATE:
		margins.minWidth = 64;
		break;
	}

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfExpandToggleNode()
{
	NodeMargins margins;

	margins.left = margins.right = margins.top = margins.bottom = 7;
	margins.minWidth = margins.charHeight = getFontSizeOfExpandToggleNode();

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfBundleNode()
{
	return getMarginsForNodeType(Node::NODE_CLASS, false);
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleForNodeType(
	Node::NodeType type, bool isActive, bool isFocused, bool hasChildren
){
	NodeStyle style;

	ColorScheme* scheme = ColorScheme::getInstance().get();

	if (isActive || isFocused)
	{
		style.color = scheme->getNodeTypeColor(type, "hover");
	}
	else
	{
		style.color = scheme->getNodeTypeColor(type);
	}

	style.textColor = scheme->getColor("graph/text");
	style.borderColor = isActive ? scheme->getColor("graph/border") : "#00000000";

	style.fontName = getFontNameForNodeType(type);
	style.fontSize = getFontSizeForNodeType(type);

	switch (type)
	{
	case Node::NODE_UNDEFINED:
		style.borderDashed = true;

	case Node::NODE_NAMESPACE:
		style.borderColor = style.color;
		style.color = "#00000000";
		style.borderWidth = 1;

		style.cornerRadius = 20;

		style.textOffset.x = 15;
		style.textOffset.y = 6;
		break;

	case Node::NODE_UNDEFINED_TYPE:
		style.hatchingColor = scheme->getColor("graph/hatching");

	case Node::NODE_STRUCT:
	case Node::NODE_CLASS:
	case Node::NODE_ENUM:
	case Node::NODE_TYPEDEF:
	case Node::NODE_TEMPLATE_PARAMETER_TYPE:
		if (isActive)
		{
			style.fontBold = true;
		}

		style.shadowColor = scheme->getColor("graph/border");
		if (isFocused)
		{
			style.shadowColor.insert(1, "FF");
		}
		else
		{
			style.shadowColor.insert(1, "80");
		}
		style.shadowBlurRadius = 5;

		if (hasChildren)
		{
			style.cornerRadius = 20;
			style.textOffset.x = 10;
			style.textOffset.y = 8;
		}
		else
		{
			style.cornerRadius = 10;
			style.textOffset.x = 8;
			style.textOffset.y = 8;
		}
		break;

	case Node::NODE_FILE:
		if (isActive)
		{
			style.fontBold = true;
		}

		style.cornerRadius = 10;
		style.textOffset.x = 6;
		style.textOffset.y = 9;
		style.iconPath = "data/gui/graph_view/images/file.png";
		style.iconSize = s_fontSize + 2;
		style.iconOffset.x = 9;
		style.iconOffset.y = 9;
		style.iconColor = scheme->getColor("graph/icon");
		break;

	case Node::NODE_UNDEFINED_FUNCTION:
	case Node::NODE_UNDEFINED_VARIABLE:
		style.hatchingColor = scheme->getColor("graph/hatching");

	case Node::NODE_FUNCTION:
	case Node::NODE_METHOD:
	case Node::NODE_GLOBAL_VARIABLE:
	case Node::NODE_FIELD:
	case Node::NODE_ENUM_CONSTANT:
		if (isActive || isFocused)
		{
			style.fontBold = true;
		}

		style.cornerRadius = 8;
		style.textOffset.x = 5;
		style.textOffset.y = 3;
		break;
	}

	if (isActive)
	{
		style.borderWidth = 1.5f;
	}

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfAccessNode()
{
	NodeStyle style;

	ColorScheme* scheme = ColorScheme::getInstance().get();

	style.color = scheme->getColor("graph/background");
	style.textColor = scheme->getColor("graph/text");
	style.iconColor = scheme->getColor("graph/icon");
	style.borderColor = "#00000000";

	style.cornerRadius = 12;

	style.fontName = getFontNameOfAccessNode();
	style.fontSize = getFontSizeOfAccessNode();
	style.fontBold = true;

	style.textOffset.x = 10;
	style.textOffset.y = 10;

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfExpandToggleNode()
{
	NodeStyle style;

	ColorScheme* scheme = ColorScheme::getInstance().get();

	style.color = scheme->getColor("graph/background");
	style.textColor = scheme->getColor("graph/text");
	style.iconColor = scheme->getColor("graph/icon");
	style.borderColor = "#00000000";

	style.cornerRadius = 100;

	style.fontName = getFontNameOfExpandToggleNode();
	style.fontSize = getFontSizeOfExpandToggleNode();

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfBundleNode(bool isFocused)
{
	NodeStyle style = getStyleForNodeType(Node::NODE_CLASS, false, isFocused, false);

	ColorScheme* scheme = ColorScheme::getInstance().get();

	style.shadowColor = "";
	style.borderColor = scheme->getColor("graph/border");
	style.borderWidth = isFocused ? 2 : 1;

	return style;
}

GraphViewStyle::EdgeStyle GraphViewStyle::getStyleForEdgeType(Edge::EdgeType type, bool isActive, bool isFocused)
{
	EdgeStyle style;

	style.width = isActive ? 3 : 1;
	style.zValue = isActive ? 5 : 2;

	style.arrowLength = 5;
	style.arrowWidth = 8;

	style.cornerRadius = 10;
	style.verticalOffset = 2;

	style.originOffset.x = 17;
	style.targetOffset.x = 17;

	style.originOffset.y = -1;
	style.targetOffset.y = 1;

	if (isActive)
	{
		style.color = ColorScheme::getInstance()->getEdgeTypeColor(type, "hover");
	}
	else
	{
		style.color = ColorScheme::getInstance()->getEdgeTypeColor(type);
	}

	switch (type)
	{
	case Edge::EDGE_AGGREGATION:
		style.isStraight = true;
		style.width = 4;
		style.zValue = isActive ? -2 : -5;
		break;
	case Edge::EDGE_CALL:
		style.originOffset.y = 1;
		style.targetOffset.y = -1;
		style.verticalOffset = 4;
		break;
	case Edge::EDGE_USAGE:
		style.originOffset.y = 3;
		style.targetOffset.y = -3;
		style.verticalOffset = 6;
		break;
	case Edge::EDGE_INHERITANCE:
		style.arrowLength = 20;
		style.arrowWidth = 14;
		style.arrowClosed = true;
		style.targetOffset.x = 34;
		break;
	default:
		break;
	}

	return style;
}

size_t GraphViewStyle::toGridSize(size_t x)
{
	size_t r = s_gridCellSize;

	while (r < x - 1)
	{
		r += s_gridCellPadding + s_gridCellSize;
	}

	return r;
}

size_t GraphViewStyle::s_gridCellSize = 5;
size_t GraphViewStyle::s_gridCellPadding = 10;

std::map<Node::NodeType, float> GraphViewStyle::s_charWidths;
std::map<Node::NodeType, float> GraphViewStyle::s_charHeights;

std::shared_ptr<GraphViewStyleImpl> GraphViewStyle::s_impl;

int GraphViewStyle::s_fontSize;
std::string GraphViewStyle::s_fontName;
