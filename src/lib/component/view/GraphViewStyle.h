#ifndef GRAPH_VIEW_STYLE_H
#define GRAPH_VIEW_STYLE_H

#include <map>
#include <memory>

#include "Vector2.h"

#include "AccessKind.h"
#include "GroupType.h"
#include "Node.h"

class GraphViewStyleImpl;

class GraphViewStyle
{
public:
	static Vec2i alignOnRaster(Vec2i position);

	struct NodeMargins
	{
		NodeMargins();

		int left;
		int right;

		int top;
		int bottom;

		int spacingX;
		int spacingY;
		int spacingA;

		int minWidth;

		float charWidth;
		float charHeight;

		int iconWidth;
	};

	struct NodeColor
	{
		std::string fill;
		std::string border;
		std::string text;
		std::string icon;
		std::string hatching;
	};

	struct NodeStyle
	{
		NodeStyle();

		NodeColor color;

		int cornerRadius;

		int borderWidth;
		bool borderDashed;

		std::string fontName;
		size_t fontSize;
		bool fontBold;

		Vec2i textOffset;

		FilePath iconPath;
		Vec2i iconOffset;
		size_t iconSize;

		bool hasHatching;
	};

	struct EdgeStyle
	{
		EdgeStyle();

		std::string color;

		float width;
		int zValue;

		int arrowLength;
		int arrowWidth;
		bool arrowClosed;

		int cornerRadius;
		int verticalOffset;

		Vec2i originOffset;
		Vec2i targetOffset;

		bool dashed;
	};

	static std::shared_ptr<GraphViewStyleImpl> getImpl();
	static void setImpl(std::shared_ptr<GraphViewStyleImpl> impl);

	static void loadStyleSettings();

	static size_t getFontSizeForStyleType(NodeType::StyleType type);
	static size_t getFontSizeOfAccessNode();
	static size_t getFontSizeOfExpandToggleNode();
	static size_t getFontSizeOfCountCircle();
	static size_t getFontSizeOfQualifier();
	static size_t getFontSizeOfTextNode(int fontSizeDiff);
	static size_t getFontSizeOfGroupNode();

	static std::string getFontNameForDataNode();
	static std::string getFontNameOfAccessNode();
	static std::string getFontNameOfExpandToggleNode();
	static std::string getFontNameOfTextNode();
	static std::string getFontNameOfGroupNode();

	static NodeMargins getMarginsForDataNode(NodeType::StyleType type, bool hasIcon, bool hasChildren);
	static NodeMargins getMarginsOfAccessNode(AccessKind access);
	static NodeMargins getMarginsOfExpandToggleNode();
	static NodeMargins getMarginsOfBundleNode();
	static NodeMargins getMarginsOfTextNode(int fontSizeDiff);
	static NodeMargins getMarginsOfGroupNode(GroupType type, bool hasName);

	static NodeStyle getStyleForNodeType(
		NodeType type,
		bool defined,
		bool isActive,
		bool isFocused,
		bool isCoFocused,
		bool hasChildren,
		bool hasQualifier);
	static NodeStyle getStyleOfAccessNode();
	static NodeStyle getStyleOfExpandToggleNode();
	static NodeStyle getStyleOfCountCircle();
	static NodeStyle getStyleOfBundleNode(bool isFocused);
	static NodeStyle getStyleOfQualifier();
	static NodeStyle getStyleOfTextNode(int fontSizeDiff);
	static NodeStyle getStyleOfGroupNode(GroupType type, bool isCoFocused);

	static EdgeStyle getStyleForEdgeType(
		Edge::EdgeType type, bool isActive, bool isFocused, bool isTrailEdge, bool isAmbiguous);

	static int toGridOffset(int x);
	static int toGridSize(int x);
	static int toGridGap(int x);

	static float getZoomFactor();

	static const std::string& getFocusColor();
	static const NodeColor& getNodeColor(const std::string& typeStr, bool highlight);
	static const std::string& getEdgeColor(const std::string& type);
	static const NodeColor& getScreenMatchColor(bool focus);

	static int s_gridCellSize;
	static int s_gridCellPadding;

private:
	static NodeStyle getStyleForNodeType(
		NodeType::StyleType type,
		const std::string& underscoredTypeString,
		const FilePath& iconPath,
		bool defined,
		bool isActive,
		bool isFocused,
		bool isCoFocused,
		bool hasChildren,
		bool hasQualifier);

	static float getCharWidth(NodeType::StyleType type);
	static float getCharHeight(NodeType::StyleType type);

	static float getCharWidth(const std::string& fontName, size_t fontSize);
	static float getCharHeight(const std::string& fontName, size_t fontSize);

	static std::map<NodeType::StyleType, float> s_charWidths;
	static std::map<NodeType::StyleType, float> s_charHeights;

	static std::shared_ptr<GraphViewStyleImpl> s_impl;

	static int s_fontSize;
	static std::string s_fontName;
	static float s_zoomFactor;

	static std::string s_focusColor;
	static std::map<std::string, NodeColor> s_nodeColors;
	static std::map<std::string, std::string> s_edgeColors;
	static std::map<bool, NodeColor> s_screenMatchColors;
};

#endif	  // GRAPH_VIEW_STYLE_H
