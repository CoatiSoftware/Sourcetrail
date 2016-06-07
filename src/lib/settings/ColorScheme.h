#ifndef COLOR_SCHEME_H
#define COLOR_SCHEME_H

#include "data/graph/Edge.h"
#include "data/graph/Node.h"
#include "settings/Settings.h"

class ColorScheme
	: public Settings
{
public:
	enum ColorState
	{
		NORMAL,
		FOCUS,
		ACTIVE
	};

	static std::shared_ptr<ColorScheme> getInstance();
	virtual ~ColorScheme();

	std::string getColor(const std::string& key) const;
	std::string getColor(const std::string& key, const std::string& defaultColor) const;

	std::string getNodeTypeColor(Node::NodeType type, const std::string& key, ColorState state) const;
	std::string getNodeTypeColor(const std::string& typeStr, const std::string& key, ColorState state) const;

	std::string getEdgeTypeColor(Edge::EdgeType type, ColorState state) const;
	std::string getEdgeTypeColor(const std::string& typeStr, ColorState state) const;

	std::string getSearchTypeColor(const std::string& searchTypeName, const std::string& key, const std::string& state = "normal") const;
	std::string getSyntaxColor(const std::string& key) const;

	std::string getCodeSelectionTypeColor(const std::string& typeStr, const std::string& key, ColorState state) const;

protected:
	ColorScheme();
	ColorScheme(const ColorScheme&) = delete;
	void operator=(const ColorScheme&) = delete;

	static std::shared_ptr<ColorScheme> s_instance;

	static std::string stateToString(ColorState state);
};

#endif // COLOR_SCHEME_H
