#ifndef QT_GRAPH_NODE_TEXT_H
#define QT_GRAPH_NODE_TEXT_H

#include "qt/view/graphElements/QtGraphNode.h"

class QtGraphNodeText
	: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeText(const std::string& name);
	virtual ~QtGraphNodeText();

	// QtGraphNode implementation
	virtual bool isTextNode() const;

	virtual void updateStyle();
};

#endif // QT_GRAPH_NODE_TEXT_H
