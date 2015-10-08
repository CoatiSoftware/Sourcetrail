#ifndef GRAPH_VIEW_STYLE_H
#define GRAPH_VIEW_STYLE_H

#include <map>
#include <memory>

#include "utility/math/Vector2.h"

#include "data/graph/Node.h"
#include "data/graph/token_component/TokenComponentAccess.h"

class GraphViewStyleImpl;

class GraphViewStyle
{
public:
	struct NodeMargins
	{
		NodeMargins();

		int left;
		int right;

		int top;
		int bottom;

		int spacingX;
		int spacingY;

		int minWidth;

		float charWidth;
		float charHeight;

		int iconWidth;
	};

	struct NodeStyle
	{
		NodeStyle();

		std::string color;
		std::string textColor;
		std::string iconColor;
		std::string hatchingColor;

		int cornerRadius;

		int borderWidth;
		std::string borderColor;
		bool borderDashed;

		std::string fontName;
		size_t fontSize;
		bool fontBold;

		Vec2i textOffset;

		std::string iconPath;
		Vec2i iconOffset;
		size_t iconSize;
	};

	struct EdgeStyle
	{
		EdgeStyle();

		std::string color;

		float width;
		int zValue;

		bool isStraight;

		int arrowLength;
		int arrowWidth;
		bool arrowClosed;

		int cornerRadius;
		int verticalOffset;

		Vec2i originOffset;
		Vec2i targetOffset;
	};

	static std::shared_ptr<GraphViewStyleImpl> getImpl();
	static void setImpl(std::shared_ptr<GraphViewStyleImpl> impl);

	static void loadStyleSettings();

	static float getCharWidthForNodeType(Node::NodeType type);
	static float getCharHeightForNodeType(Node::NodeType type);

	static size_t getFontSizeForNodeType(Node::NodeType type);
	static size_t getFontSizeOfAccessNode();
	static size_t getFontSizeOfExpandToggleNode();
	static size_t getFontSizeOfCountCircle();

	static std::string getFontNameForNodeType(Node::NodeType type);
	static std::string getFontNameOfAccessNode();
	static std::string getFontNameOfExpandToggleNode();

	static NodeMargins getMarginsForNodeType(Node::NodeType type, bool hasChildren);
	static NodeMargins getMarginsOfAccessNode(TokenComponentAccess::AccessType type);
	static NodeMargins getMarginsOfExpandToggleNode();
	static NodeMargins getMarginsOfBundleNode();

	static NodeStyle getStyleForNodeType(Node::NodeType type, bool defined, bool isActive, bool isFocused, bool hasChildren);
	static NodeStyle getStyleOfAccessNode();
	static NodeStyle getStyleOfExpandToggleNode();
	static NodeStyle getStyleOfBundleNode(bool isFocused);

	static EdgeStyle getStyleForEdgeType(Edge::EdgeType type, bool isActive, bool isFocused);

	static int toGridOffset(int x);
	static int toGridSize(int x);
	static int toGridGap(int x);

	static float getZoomFactor();

	static int s_gridCellSize;
	static int s_gridCellPadding;

private:
	static void addIcon(Node::NodeType type, bool hasChildren, NodeStyle* style);

	static std::map<Node::NodeType, float> s_charWidths;
	static std::map<Node::NodeType, float> s_charHeights;

	static std::shared_ptr<GraphViewStyleImpl> s_impl;

	static int s_fontSize;
	static std::string s_fontName;
	static float s_zoomFactor;
};

#endif // GRAPH_VIEW_STYLE_H
