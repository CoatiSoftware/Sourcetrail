#include "component/view/GraphViewStyle.h"

#include "utility/logging/logging.h"

#include "utility/ResourcePaths.h"

#include "component/view/GraphViewStyleImpl.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

int GraphViewStyle::s_gridCellSize = 5;
int GraphViewStyle::s_gridCellPadding = 10;

std::unordered_map<NodeType::StyleType, float> GraphViewStyle::s_charWidths;
std::unordered_map<NodeType::StyleType, float> GraphViewStyle::s_charHeights;

std::shared_ptr<GraphViewStyleImpl> GraphViewStyle::s_impl;

int GraphViewStyle::s_fontSize;
std::string GraphViewStyle::s_fontName;
float GraphViewStyle::s_zoomFactor;

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

		if (std::abs(r) > rasterPosDivisor/2)
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

		if (std::abs(r) > rasterPosDivisor/2)
		{
			if(t != 0)
			{
				t += (t / std::abs(t));
			}
			else if(r != 0)
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
	s_zoomFactor = (ApplicationSettings::getInstance()->getFontSize()) / float(s_fontSize) * zoomDifference;

	s_charWidths.clear();
	s_charHeights.clear();

	s_nodeColors.clear();
	s_edgeColors.clear();
	s_screenMatchColors.clear();

	s_gridCellPadding = getImpl()->getCharHeight(NodeType::STYLE_BIG_NODE) - 8;
	s_gridCellSize = s_gridCellPadding / 2;
}

size_t GraphViewStyle::getFontSizeForStyleType(NodeType::StyleType type)
{
	switch (type)
	{
	case NodeType::STYLE_PACKAGE:
	case NodeType::STYLE_SMALL_NODE:
		return s_fontSize - 3;
	}
	return s_fontSize;
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

size_t GraphViewStyle::getFontSizeOfTextNode()
{
	return s_fontSize + 5;
}

std::string GraphViewStyle::getFontNameForDataNode()
{
	return s_fontName;
}

std::string GraphViewStyle::getFontNameOfAccessNode()
{
	return "Fira Sans";
}

std::string GraphViewStyle::getFontNameOfExpandToggleNode()
{
	return "Fira Sans";
}

std::string GraphViewStyle::getFontNameOfTextNode()
{
	return "Fira Sans";
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsForDataNode(NodeType::StyleType type, bool hasIcon, bool hasChildren)
{
	NodeMargins margins;
	margins.spacingX = 6;
	margins.spacingY = 8;

	switch (type)
	{
	case NodeType::STYLE_PACKAGE:
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
		margins.minWidth = 56;
		break;
	case ACCESS_PROTECTED:
		margins.minWidth = 72;
		break;
	case ACCESS_PRIVATE:
		margins.minWidth = 58;
		break;
	case ACCESS_DEFAULT:
		margins.minWidth = 60;
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
	margins.minWidth = margins.charHeight = getFontSizeOfExpandToggleNode();

	return margins;
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfBundleNode()
{
	return getMarginsForDataNode(NodeType::STYLE_BIG_NODE, true, false);
}

GraphViewStyle::NodeMargins GraphViewStyle::getMarginsOfTextNode()
{
	NodeMargins margins;

	margins.left = margins.right = 0;
	margins.top = margins.bottom = 6;
	margins.minWidth = margins.charHeight = getFontSizeOfTextNode();

	return margins;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleForNodeType(
	NodeType type, bool defined, bool isActive, bool isFocused, bool hasChildren, bool hasQualifier
){
	return getStyleForNodeType(
		type.getNodeStyle(),
		type.getUnderscoredTypeString(),
		type.getIconPath(),
		defined,
		isActive,
		isFocused,
		hasChildren,
		hasQualifier
	);
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleForNodeType(
	NodeType::StyleType type, const std::string& underscoredTypeString, 
	const FilePath& iconPath, bool defined, bool isActive, bool isFocused, 
	bool hasChildren, bool hasQualifier
){
	NodeStyle style;

	style.color = getNodeColor(underscoredTypeString, isActive || isFocused);

	style.fontName = getFontNameForDataNode();
	style.fontSize = getFontSizeForStyleType(type);

	if (isActive || isFocused)
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
		ResourcePaths::getGuiPath().concat(FilePath("graph_view/images/bundle.png")), 
		true, 
		false, 
		isFocused, 
		false, 
		false
	);
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfQualifier()
{
	NodeStyle style;

	style.color = getNodeColor("qualifier", false);
	style.borderWidth = 2;

	return style;
}

GraphViewStyle::NodeStyle GraphViewStyle::getStyleOfTextNode()
{
	NodeStyle style;

	style.color = getNodeColor("text", false);

	style.fontName = getFontNameOfTextNode();
	style.fontSize = getFontSizeOfTextNode();
	style.fontBold = true;

	style.textOffset.y = 10;

	return style;
}

GraphViewStyle::EdgeStyle GraphViewStyle::getStyleForEdgeType(
	Edge::EdgeType type, bool isActive, bool isFocused, bool isTrailEdge)
{
	EdgeStyle style;

	style.width = isActive ? 4 : 2;
	style.zValue = isActive ? 5 : 2;

	style.arrowLength = 5;
	style.arrowWidth = 8;

	style.cornerRadius = 10;
	style.verticalOffset = 2;

	style.originOffset.x = 17;
	style.targetOffset.x = 17;

	style.originOffset.y = -1;
	style.targetOffset.y = 1;

	style.color = getEdgeColor(Edge::getUnderscoredTypeString(type), isActive || isFocused);

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
		style.zValue = isActive ? 1 : -5;
		break;
	case Edge::EDGE_CALL:
		style.originOffset.y = 1;
		style.targetOffset.y = -1;
		style.verticalOffset = 4;

		if (isTrailEdge && isActive)
		{
			style.width = 3;
			style.color = ColorScheme::getInstance()->getColor(
				"graph/edge/" + Edge::getUnderscoredTypeString(type) + "/trail_focus", style.color);
		}
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
		style.zValue = isActive ? 2 : -3;
		break;
	case Edge::EDGE_INCLUDE:
	case Edge::EDGE_MACRO_USAGE:
		style.zValue = isActive ? 2 : -3;
	default:
		break;
	}

	if (isTrailEdge)
	{
		style.zValue = isActive ? 5 : 2;
	}

	return style;
}

int GraphViewStyle::toGridOffset(int x)
{
	if (x < 1)
	{
		return 0;
	}

	int r = 0;

	while (r < x - 1)
	{
		r += s_gridCellPadding + s_gridCellSize;
	}

	return r;
}

int GraphViewStyle::toGridSize(int x)
{
	if (x < 1)
	{
		return 0;
	}

	return s_gridCellSize + toGridOffset(x - s_gridCellSize);
}

int GraphViewStyle::toGridGap(int x)
{
	if (x < 1)
	{
		return 0;
	}

	return s_gridCellPadding + toGridOffset(x - s_gridCellPadding);
}

float GraphViewStyle::getZoomFactor()
{
	return s_zoomFactor;
}

const GraphViewStyle::NodeColor& GraphViewStyle::getNodeColor(const std::string& typeStr, bool focus)
{
	std::string type = focus ? typeStr + "focus" : typeStr;
	std::map<std::string, NodeColor>::const_iterator it = s_nodeColors.find(type);

	if (it != s_nodeColors.end())
	{
		return it->second;
	}

	NodeColor color;
	ColorScheme* scheme = ColorScheme::getInstance().get();
	ColorScheme::ColorState state = focus ? ColorScheme::FOCUS : ColorScheme::NORMAL;

	color.fill = scheme->getNodeTypeColor(typeStr, "fill", state);
	color.border = scheme->getNodeTypeColor(typeStr, "border", state);
	color.text = scheme->getNodeTypeColor(typeStr, "text", state);
	color.icon = scheme->getNodeTypeColor(typeStr, "icon", state);
	color.hatching = scheme->getNodeTypeColor(typeStr, "hatching", state);

	s_nodeColors.emplace(type, color);

	return s_nodeColors.find(type)->second;
}

const std::string& GraphViewStyle::getEdgeColor(const std::string& typeStr, bool focus)
{
	std::string type = focus ? typeStr + "focus" : typeStr;
	std::map<std::string, std::string>::const_iterator it = s_edgeColors.find(type);

	if (it != s_edgeColors.end())
	{
		return it->second;
	}

	ColorScheme* scheme = ColorScheme::getInstance().get();
	ColorScheme::ColorState state = focus ? ColorScheme::FOCUS : ColorScheme::NORMAL;
	std::string color = scheme->getEdgeTypeColor(typeStr, state);

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
	std::unordered_map<NodeType::StyleType, float>::const_iterator it = s_charWidths.find(type);

	if (it != s_charWidths.end())
	{
		return it->second;
	}

	float charWidth = getImpl()->getCharWidth(type);
	s_charWidths.emplace(type, charWidth);
	return charWidth;
}

float GraphViewStyle::getCharHeight(NodeType::StyleType type)
{
	std::unordered_map<NodeType::StyleType, float>::const_iterator it = s_charHeights.find(type);

	if (it != s_charHeights.end())
	{
		return it->second;
	}

	float charHeight = getImpl()->getCharHeight(type);
	s_charHeights.emplace(type, charHeight);
	return charHeight;
}
