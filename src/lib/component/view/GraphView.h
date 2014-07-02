#ifndef GRAPH_VIEW_H
#define GRAPH_VIEW_H

#include "component/view/View.h"

class GraphView : public View
{
public:
	GraphView(ViewLayout* viewLayout);
	virtual ~GraphView();

	virtual std::string getName() const;

	virtual void addNode(const Vec2i& position, const std::string& name) = 0;
};

#endif // GRAPH_VIEW_H