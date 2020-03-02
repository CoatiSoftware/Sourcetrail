#ifndef QT_GRAPH_NODE_DATA_H
#define QT_GRAPH_NODE_DATA_H

#include "QtGraphNode.h"

class FilePath;

class QtGraphNodeData: public QtGraphNode
{
	Q_OBJECT
public:
	QtGraphNodeData(
		GraphFocusHandler* focusHandler,
		const Node* data,
		const std::wstring& name,
		bool childVisible,
		bool hasQualifier,
		bool isInteractive);
	virtual ~QtGraphNodeData();

	const Node* getData() const;
	FilePath getFilePath() const;

	// QtGraphNode implementation
	virtual bool isDataNode() const;

	virtual Id getTokenId() const;

	virtual void onClick();
	virtual void onMiddleClick();
	virtual void updateStyle();

protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
	const Node* m_data;
	bool m_childVisible;
	bool m_hasQualifier;
};

#endif	  // QT_GRAPH_NODE_DATA_H
