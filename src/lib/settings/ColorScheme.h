#ifndef COLOR_SCHEME_H
#define COLOR_SCHEME_H

#include "data/graph/Node.h"
#include "data/graph/Edge.h"
#include "settings/Settings.h"

class ColorScheme
	: public Settings
{
public:
	static std::shared_ptr<ColorScheme> getInstance();
	virtual ~ColorScheme();

	std::string getColor(const std::string& key) const;

	std::string getNodeTypeColor(Node::NodeType type, const std::string& state = "normal") const;
	std::string getEdgeTypeColor(Edge::EdgeType type, const std::string& state = "normal") const;
	std::string getSearchTypeColor(const std::string& searchTypeName, const std::string& state = "normal") const;
	std::string getSyntaxColor(const std::string& key) const;

protected:
	ColorScheme();
	ColorScheme(const ColorScheme&);
	void operator=(const ColorScheme&);

	static std::shared_ptr<ColorScheme> s_instance;
};

#endif // COLOR_SCHEME_H
