#ifndef QT_GRAPH_NODE_TEXT_H
#define QT_GRAPH_NODE_TEXT_H

#include "QtGraphNode.h"

class QtGraphNodeText: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeText(const std::wstring& name, int fontSizeDiff);
	virtual ~QtGraphNodeText();

	// QtGraphNode implementation
	virtual bool isTextNode() const;

	virtual void updateStyle();

private:
	int m_fontSizeDiff;
};

#endif	  // QT_GRAPH_NODE_TEXT_H
