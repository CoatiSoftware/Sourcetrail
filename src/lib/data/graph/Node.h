#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "data/graph/Edge.h"
#include "data/graph/Token.h"

class Node: public Token
{
public:
	enum NodeType
	{
		NODE_UNDEFINED,
		NODE_CLASS,
		NODE_STRUCT,
		NODE_GLOBAL_VARIABLE,
		NODE_FIELD,
		NODE_FUNCTION,
		NODE_METHOD,
		NODE_NAMESPACE,
		NODE_ENUM,
		NODE_TYPEDEF
	};

	Node(NodeType type, const std::string& name);
	virtual ~Node();

	std::shared_ptr<Node> createPlainCopy() const;

	NodeType getType() const;
	void setType(NodeType type);

	const std::string& getName() const;
	const std::vector<Edge*>& getEdges() const;

	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);

	Node* getParentNode() const;
	Edge* getMemberEdge() const;

	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Edge* findEdgeOfType(Edge::EdgeType type) const;
	Edge* findEdgeOfType(Edge::EdgeType type, std::function<bool(Edge*)> func) const;

	void forEachEdge(std::function<void(Edge*)> func) const;
	void forEachEdgeOfType(Edge::EdgeType type, std::function<void(Edge*)> func) const;

	// Token implementation.
	virtual bool isNode() const;
	virtual bool isEdge() const;

	// Additional field accessors for different NodeTypes.
	void setAccess(Edge::AccessType access);

	bool isConst() const;
	void setConst(bool isConst);

	bool isStatic() const;
	void setStatic(bool isStatic);

	std::string getSignature() const;
	void setSignature(const std::string& signature);

	// Logging.
	std::string getTypeString(NodeType type) const;
	std::string getAsString() const;

private:
	// Constructor for plain copies.
	Node(Id id, NodeType type, const std::string& name);

	NodeType m_type;
	std::string m_name;

	std::vector<Edge*> m_edges;

	// Additional fields for different NodeTypes.
	bool m_isConst;
	bool m_isStatic;

	std::string m_signature;
};

std::ostream& operator<<(std::ostream& ostream, const Node& node);

#endif // NODE_H
