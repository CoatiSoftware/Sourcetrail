#include "GraphViewStyle.h"

#include "ApplicationSettings.h"
#include "ColorScheme.h"
#include "GraphViewStyleImpl.h"
#include "ResourcePaths.h"
#include "logging.h"
#include "utilityString.h"

int GraphViewStyle::s_gridCellSize = 5;
int GraphViewStyle::s_gridCellPadding = 10;

std::map<NodeType::StyleType, float> GraphViewStyle::s_charWidths;
std::map<NodeType::StyleType, float> GraphViewStyle::s_charHeights;

std::shared_ptr<GraphViewStyleImpl> GraphViewStyle::s_impl;

int GraphViewStyle::s_fontSize;
std::string GraphViewStyle::s_fontName;
float GraphViewStyle::s_zoomFactor;

std::string GraphViewStyle::s_focusColor;
std::map<std::string, GraphViewStyle::NodeColor> GraphViewStyle::s_nodeColors;
std::map<std::string, std::string> GraphViewStyle::s_edgeColors;
std::map<bool, GraphViewStyle::NodeColor> GraphViewStyle::s_screenMatchColors;

Vec2i GraphViewStyle::alignOnRaster(Vec2i position)
{
	int rasterPosDivisor = s_gridCellSize + s_gridCellPadding;

	if (position.x % rasterPosDivisor != 0)
	{
		int t = position.x / rasterPosDivisor;
		int r = position.x % rasterPosDivisor;

		if (std::abs(r) > rasterPosDivisor / 2)
		{
			if (t != 0)
			{
				t += (t / std::abs(t));
			}
			else if (r != 0)
			{
				t += (r / std::abs(r));
			}
		}

		position.x = t * rasterPosDivisor;
	}

	if (position.y % rasterPosDivisor != 0)
	{
		int t = position.y / rasterPosDivisor;
		int r = position.y % rasterPosDivisor;

		if (std::abs(r) > rasterPosDivisor / 2)
		{
			if (t != 0)
			{
				t += (t / std::abs(t));
			}
			else if (r != 0)
			{
				t += (r / std::abs(r));
			}
		}

		position.y = t * rasterPosDivisor;
	}

	return position;
}

GraphViewStyle::NodeMargins::NodeMargins()
	: left(0)
	, right(0)
	, top(0)
	, bottom(0)
	, spacingX(0)
	, spacingY(0)
	, spacingA(0)
	, minWidth(0)
	, charWidth(0.0f)
	, charHeight(0.0f)
	, iconWidth(0)
{
}

GraphViewStyle::NodeStyle::NodeStyle()
	: cornerRadius(0)
	, borderWidth(0)
	, borderDashed(false)
	, fontSize(0)
	, fontBold(false)
	, iconSize(0)
	, hasHatching(false)
{
}

GraphViewStyle::EdgeStyle::EdgeStyle()
	: width(0)
	, zValue(0)
	, arrowLength(0)
	, arrowWidth(0)
	, arrowClosed(false)
	, cornerRadius(0)
	, verticalOffset(0)
	, dashed(false)
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
	if (!s_impl)
	{
		return;
	}

	s_fontSize = 14;
	s_fontName = ApplicationSettings::getInstance()->getFontName();

	float zoomDifference = getImpl()->getGraphViewZoomDifferenceForPlatform();
	s_zoomFactor = (ApplicationSettings::getInstance()->getFontSize()) / float(s_fontSize) *
		zoomDifference;

	s_charWidths.clear();
	s_charHeights.clear();

	s_focusColor.clear();
	s_nodeColors.clear();
	s_edgeColors.clear();
	s_screenMatchColors.clear();

	s_gridCellPadding = static_cast<int>(getCharHeight(NodeType::STYLE_BIG_NODE) - 8);
	s_gridCellSize = s_gridCellPadding / 2;
}

size_t GraphViewStyle::getFontSizeForStyleType(NodeType::StyleType type)
{
	switch (type)
	{
	case NodeType::STYLE_PACKAGE:
	case NodeType::STYLE_SMALL_NODE:
		return s_fontSize - 3;

	case NodeType::STYLE_GROUP:
		return s_fontSize - 2;

	default:
		return s_fontSize;
	}
}

size_t GraphViewStyle::getFontSizeOfAccessNode()
{
	return s_fontSize - 2;
}

size_t GraphViewStyle::getFontSizeOfExpandToggleNode()
{
	return s_fontSize - 3;
}

size_t GraphViewStyle::getFontSizeOfCountCircle()
{
	return s_fontSize - 3;
}

size_t GraphViewStyle::getFontSizeOfQualifier()
{
	return s_fontSize - 3;
}

size_t GraphViewStyle::getFontSizeOfTextNode(int fontSizeDiff)
{
	return s_fontSize + fontSizeDiff;
}

size_t GraphViewStyle::getFontSizeOfGroupNode()
{
	return getFontSizeForStyleType(NodeType::STYLE_GROUP);
}

std::string GraphViewStyle::getFontNameForDataNode()
{
	return s_fontName + ", consolas, monospace, sans-serif";
}

std::string GraphViewStyle::getFontNameOfAccessNode()
{
	return "Fira Sans, sans-serif";
}

std::string GraphViewStyle::getFontNameOfExpandToggleNode()
{
	return "Fira Sans, sans-serif";
}

std::string GraphViewStyle::getFontNameOfTextNode()
{
	return "Fira Sans, sans-serif";
}

std::string GraphViewStyle::getFontNameOfGroupNode()
{
	return s_fontName + ", consolas, monospace, sans-serif";
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsForDataNode(
	NodeType::StyleType type, bool hasIcon, bool hasChildren)
{
	NodeMargins margins;
	margins.spacingX = 6;
	margins.spacingY = 8;

	switch (type)
	{
	case NodeType::STYLE_PACKAGE:
	case NodeType::STYLE_GROUP:
		margins.left = margins.right = 5;
		margins.top = margins.bottom = 3;
		margins.iconWidth = s_fontSize - 3;
		break;
	case NodeType::STYLE_BIG_NODE:
		if (hasIcon)
		{
			margins.iconWidth = s_fontSize + 11;
		}

		if (hasChildren)
		{
			margins.left = margins.right = 10;
			margins.top = margins.bottom = 10;
			margins.spacingA = 5;
		}
		else
		{
			margins.left = margins.right = 8;
			margins.top = margins.bottom = 8;
		}
		break;
	case NodeType::STYLE_SMALL_NODE:
		if (hasChildren)
		{
			margins.top = margins.bottom = 5;
			margins.spacingY = 4;
			margins.spacingA = 3;
		}
		else
		{
			margins.top = margins.bottom = 3;
		}

		margins.left = margins.right = 5;
		break;
	}

	margins.charWidth = getCharWidth(type);
	margins.charHeight = getCharHeight(type);

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfAccessNode(AccessKind access)
{
	NodeMargins margins;
	margins.spacingX = margins.spacingY = 8;

	margins.left = margins.right = 10;
	margins.top = 40;
	margins.bottom = 10;

	switch (access)
	{
	case ACCESS_NONE:
		margins.top = 10;
		margins.minWidth = 30;
		break;
	case ACCESS_PUBLIC:
		margins.minWidth = 57;
		break;
	case ACCESS_PROTECTED:
		margins.minWidth = 78;
		break;
	case ACCESS_PRIVATE:
		margins.minWidth = 62;
		break;
	case ACCESS_DEFAULT:
		margins.minWidth = 62;
		break;
	case ACCESS_TEMPLATE_PARAMETER:
	case ACCESS_TYPE_PARAMETER:
		margins.minWidth = 133;
		break;
	}

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfExpandToggleNode()
{
	NodeMargins margins;

	margins.left = margins.right = margins.top = margins.bottom = 6;
	margins.charHeight = static_cast<float>(getFontSizeOfExpandToggleNode());
	margins.minWidth = static_cast<int>(margins.charHeight);

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfBundleNode()
{
	return getMarginsForDataNode(NodeType::STYLE_BIG_NODE, true, false);
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfTextNode(int fontSizeDiff)
{
	NodeMargins margins;

	margins.left = margins.right = 0;
	margins.top = margins.bottom = 6;
	margins.charHeight = static_cast<float>(getFontSizeOfTextNode(fontSizeDiff));
	margins.minWidth = static_cast<int>(margins.charHeight);

	margins.charWidth = getCharWidth(getFontNameOfTextNode(), getFontSizeOfTextNode(fontSizeDiff));

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfGroupNode(GroupType type, bool hasName)
{
	NodeMargins margins;
	margins.spacingX = margins.spacingY = GraphViewStyle::s_gridCellPadding;

	if (type == GroupType::FRAMELESS)
	{
		return margins;
	}

	margins.spacingA = (hasName ? 14 : 0);

	margins.left = margins.right = 26;
	margins.top = (hasName ? 12 : 20);
	margins.bottom = 20;

	if (hasName)
	{
		margins.charHeight = static_cast<float>(getFontSizeOfGroupNode());
		margins.minWidth = static_cast<int>(margins.charHeight);
	}

	margins.charWidth = getCharWidth(NodeType::STYLE_GROUP);

	return margins;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleForNodeType(
	NodeType type, bool defined, bool isActive, bool isFocused, bool isCoFocused, bool hasChildren, bool hasQualifier)
{
	return getStyleForNodeType(
		type.getNodeStyle(),
		type.getUnderscoredTypeString(),
		type.getIconPath(),
		defined,
		isActive,
		isFocused,
		isCoFocused,
		hasChildren,
		hasQualifier);
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleForNodeType(
	NodeType::StyleType type,
	const std::string& underscoredTypeString,
	const FilePath& iconPath,
	bool defined,
	bool isActive,
	bool isFocused,
	bool isCoFocused,
	bool hasChildren,
	bool hasQualifier)
{
	NodeStyle style;

	style.color = getNodeColor(underscoredTypeString, isActive || isCoFocused);

	style.fontName = getFontNameForDataNode();
	style.fontSize = getFontSizeForStyleType(type);

	if (isActive || isCoFocused)
	{
		style.fontBold = true;
	}

	if (isActive)
	{
		style.borderWidth = 2;
	}
	else
	{
		style.borderWidth = 1;
	}

	switch (type)
	{
	case NodeType::STYLE_PACKAGE:
	case NodeType::STYLE_GROUP:
		style.cornerRadius = 0;
		style.textOffset.x = 5;
		style.textOffset.y = 3;
		break;

	case NodeType::STYLE_BIG_NODE:
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

	case NodeType::STYLE_SMALL_NODE:
		style.cornerRadius = 8;
		style.textOffset.x = 5;
		style.textOffset.y = 3;
		break;
	}

	style.hasHatching = !defined;

	if (!iconPath.empty())
	{
		style.iconPath = iconPath;
		if (type == NodeType::STYLE_PACKAGE)
		{
			style.iconSize = s_fontSize - 4;
			style.iconOffset.x = -1;
			style.iconOffset.y = 5;
		}
		else
		{
			style.iconSize = s_fontSize + 2;

			if (hasChildren)
			{
				style.iconOffset.x = 11;
				style.textOffset.x = 6;
			}
			else
			{
				style.iconOffset.x = 9;
			}

			style.iconOffset.y = 9;
		}
	}

	if (hasQualifier)
	{
		if (!style.iconPath.empty())
		{
			style.iconOffset.x = style.iconOffset.x + 5;
		}
		else
		{
			style.textOffset.x = style.textOffset.x + 5;
		}
	}

	if (isFocused)
	{
		style.color.border = getFocusColor();
		style.borderWidth = 3;
	}

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfAccessNode()
{
	NodeStyle style;

	style.color = getNodeColor("access", false);

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

	style.color = getNodeColor("access", false);

	style.cornerRadius = 100;

	style.fontName = getFontNameOfExpandToggleNode();
	style.fontSize = getFontSizeOfExpandToggleNode();

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfCountCircle()
{
	NodeStyle style;

	style.color = getNodeColor("count_number", false);

	style.fontName = getFontNameOfExpandToggleNode();
	style.fontSize = getFontSizeOfCountCircle();

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfBundleNode(bool isFocused)
{
	return getStyleForNodeType(
		NodeType::STYLE_BIG_NODE,
		"bundle",
		ResourcePaths::getGuiPath().concatenate(L"graph_view/images/bundle.png"),
		true,
		false,
		isFocused,
		isFocused,
		false,
		false);
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfQualifier()
{
	NodeStyle style;

	style.color = getNodeColor("qualifier", false);
	style.borderWidth = 2;

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfTextNode(int fontSizeDiff)
{
	NodeStyle style;

	style.color = getNodeColor("text", false);

	style.fontName = getFontNameOfTextNode();
	style.fontSize = getFontSizeOfTextNode(fontSizeDiff);
	style.fontBold = true;

	style.textOffset.y = 10;

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfGroupNode(GroupType type, bool isCoFocused)
{
	NodeStyle style;

	style.cornerRadius = 15;
	style.borderWidth = 2;

	std::string colorType = "group/";
	if (type == GroupType::DEFAULT)
	{
		colorType += "default";
	}
	else if (type == GroupType::FILE)
	{
		colorType += "file";
	}
	else if (type == GroupType::NAMESPACE)
	{
		colorType += "namespace";
	}
	else if (type == GroupType::INHERITANCE)
	{
		colorType += "inheritance";

		if (isCoFocused)
		{
			style.borderWidth = 3;
		}
	}
	else
	{
		return style;
	}

	style.color = getNodeColor(colorType, isCoFocused);

	style.fontName = getFontNameOfGroupNode();
	style.fontSize = getFontSizeOfGroupNode();
	style.fontBold = true;

	style.textOffset.x = 12;
	style.textOffset.y = 5;

	return style;
}

GraphViewStyle::EdgeStyle GraphViewStyle::getStyleForEdgeType(
	Edge::EdgeType type, bool isActive, bool isFocused, bool isTrailEdge, bool isAmbiguous)
{
	EdgeStyle style;

	bool active = isActive || isFocused;

	style.width = active ? 4.0f : 2.0f;
	style.zValue = active ? 5 : 2;

	if (isTrailEdge)
	{
		style.zValue = active ? 5 : 2;
	}

	style.arrowLength = 5;
	style.arrowWidth = 8;

	style.cornerRadius = 10;
	style.verticalOffset = 2;

	style.originOffset.x = 17;
	style.targetOffset.x = 17;

	style.originOffset.y = 5;
	style.targetOffset.y = -5;

	if (isFocused)
	{
		style.color = getFocusColor();
	}
	else
	{
		style.color = getEdgeColor(utility::encodeToUtf8(Edge::getUnderscoredTypeString(type)));
	}

	switch (type)
	{
	case Edge::EDGE_AGGREGATION:
		style.width = 3;
		style.arrowLength = 7;
		style.arrowWidth = 10;
		style.originOffset.x = 24;
		style.targetOffset.x = 24;
		style.originOffset.y = 0;
		style.targetOffset.y = 0;
		style.verticalOffset = 0;
		style.zValue = active ? 1 : -5;
		break;

	case Edge::EDGE_CALL:
		style.originOffset.y = 3;
		style.targetOffset.y = -3;
		style.verticalOffset = 4;

		if (isTrailEdge && isActive)
		{
			style.width = 3;
			style.color = ColorScheme::getInstance()->getColor("graph/edge/call_trail_focus", style.color);
		}
		break;

	case Edge::EDGE_USAGE:
	case Edge::EDGE_OVERRIDE:
		style.originOffset.y = 1;
		style.targetOffset.y = -1;
		style.verticalOffset = 6;
		break;

	case Edge::EDGE_INHERITANCE:
		style.arrowLength = 17;
		style.arrowWidth = 14;
		style.arrowClosed = true;
		style.originOffset.x = 7;
		style.targetOffset.x = 34;
		style.originOffset.y = 10;
		style.targetOffset.y = -10;
		style.verticalOffset = 0;
		style.cornerRadius = 7;
		style.zValue = active ? 2 : -3;
		style.width = active ? 3.0f : 2.0f;

		if (isTrailEdge)
		{
			style.zValue = active ? 2 : -20;
		}
		break;

	case Edge::EDGE_TEMPLATE_SPECIALIZATION:
		style.zValue = active ? 2 : -3;
		style.arrowLength = 10;
		style.arrowWidth = 13;
		style.arrowClosed = true;
		style.targetOffset.x = 25;
		break;

	case Edge::EDGE_INCLUDE:
		style.zValue = active ? 2 : -3;
	case Edge::EDGE_MACRO_USAGE:
		style.originOffset.y = 0;
		style.targetOffset.y = 0;
	default:
		break;
	}

	if (isAmbiguous)
	{
		style.dashed = true;
	}

	return style;
}

int GraphViewStyle::toGridOffset(int x)
{
	if (x > 0)
	{
		return static_cast<int>(
			std::ceil(x / double(s_gridCellPadding + s_gridCellSize)) *
			(s_gridCellPadding + s_gridCellSize));
	}
	else
	{
		return static_cast<int>(
			std::floor(x / double(s_gridCellPadding + s_gridCellSize)) *
			(s_gridCellPadding + s_gridCellSize));
	}
}

int GraphViewStyle::toGridSize(int x)
{
	return s_gridCellSize + toGridOffset(x - s_gridCellSize);
}

int GraphViewStyle::toGridGap(int x)
{
	return s_gridCellPadding + toGridOffset(x - s_gridCellPadding);
}

float GraphViewStyle::getZoomFactor()
{
	return s_zoomFactor;
}

const std::string& GraphViewStyle::getFocusColor()
{
	if (s_focusColor.empty())
	{
		s_focusColor = ColorScheme::getInstance()->getColor("window/focus");
	}

	return s_focusColor;
}

const GraphViewStyle::NodeColor& GraphViewStyle::getNodeColor(const std::string& typeStr, bool highlight)
{
	std::string type = highlight ? typeStr + "highlight" : typeStr;
	std::map<std::string, NodeColor>::const_iterator it = s_nodeColors.find(type);

	if (it != s_nodeColors.end())
	{
		return it->second;
	}

	NodeColor color;
	ColorScheme* scheme = ColorScheme::getInstance().get();

	color.fill = scheme->getNodeTypeColor(typeStr, "fill", highlight);
	color.border = scheme->getNodeTypeColor(typeStr, "border", highlight);
	color.text = scheme->getNodeTypeColor(typeStr, "text", highlight);
	color.icon = scheme->getNodeTypeColor(typeStr, "icon", highlight);
	color.hatching = scheme->getNodeTypeColor(typeStr, "hatching", highlight);

	s_nodeColors.emplace(type, color);

	return s_nodeColors.find(type)->second;
}

const std::string& GraphViewStyle::getEdgeColor(const std::string& type)
{
	std::map<std::string, std::string>::const_iterator it = s_edgeColors.find(type);
	if (it != s_edgeColors.end())
	{
		return it->second;
	}

	std::string color = ColorScheme::getInstance()->getEdgeTypeColor(type);
	s_edgeColors.emplace(type, color);

	return s_edgeColors.find(type)->second;
}

const GraphViewStyle::NodeColor& GraphViewStyle::getScreenMatchColor(bool focus)
{
	auto it = s_screenMatchColors.find(focus);
	if (it != s_screenMatchColors.end())
	{
		return it->second;
	}

	NodeColor color;
	ColorScheme* scheme = ColorScheme::getInstance().get();

	ColorScheme::ColorState state = focus ? ColorScheme::FOCUS : ColorScheme::NORMAL;

	color.fill = scheme->getCodeAnnotationTypeColor("screen_search", "fill", state);
	color.border = scheme->getCodeAnnotationTypeColor("screen_search", "border", state);
	color.text = scheme->getCodeAnnotationTypeColor("screen_search", "text", state);

	s_screenMatchColors.emplace(focus, color);

	return s_screenMatchColors.find(focus)->second;
}

float GraphViewStyle::getCharWidth(NodeType::StyleType type)
{
	std::map<NodeType::StyleType, float>::const_iterator it = s_charWidths.find(type);
	if (it != s_charWidths.end())
	{
		return it->second;
	}

	float charWidth = getCharWidth(getFontNameForDataNode(), getFontSizeForStyleType(type));
	s_charWidths.emplace(type, charWidth);
	return charWidth;
}

float GraphViewStyle::getCharHeight(NodeType::StyleType type)
{
	std::map<NodeType::StyleType, float>::const_iterator it = s_charHeights.find(type);
	if (it != s_charHeights.end())
	{
		return it->second;
	}

	float charHeight = getCharHeight(getFontNameForDataNode(), getFontSizeForStyleType(type));
	s_charHeights.emplace(type, charHeight);
	return charHeight;
}

float GraphViewStyle::getCharWidth(const std::string& fontName, size_t fontSize)
{
	return getImpl()->getCharWidth(fontName, fontSize);
}

float GraphViewStyle::getCharHeight(const std::string& fontName, size_t fontSize)
{
	return getImpl()->getCharHeight(fontName, fontSize);
}
