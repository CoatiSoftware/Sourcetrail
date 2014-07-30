#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "data/graph/Edge.h"
#include "data/graph/Token.h"

class TokenComponentConst;
class TokenComponentStatic;
class TokenComponentSignature;

class Node: public Token
{
public:
	enum NodeType
	{
		NODE_UNDEFINED,
		NODE_UNDEFINED_FUNCTION,
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
	Node(const Node& other);
	virtual ~Node();

	NodeType getType() const;
	void setType(NodeType type);

	const std::string& getName() const;
	std::string getFullName() const;

	const std::vector<Edge*>& getEdges() const;

	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);

	Node* getParentNode() const;
	Edge* getMemberEdge() const;

	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Edge* findEdgeOfType(Edge::EdgeType type) const;
	Edge* findEdgeOfType(Edge::EdgeType type, std::function<bool(Edge*)> func) const;
	Node* findChildNode(std::function<bool(Node*)> func) const;

	void forEachEdge(std::function<void(Edge*)> func) const;
	void forEachEdgeOfType(Edge::EdgeType type, std::function<void(Edge*)> func) const;
	void forEachChildNode(std::function<void(Node*)> func) const;

	// Token implementation.
	virtual bool isNode() const;
	virtual bool isEdge() const;

	// Component setters.
	void addComponentConst(std::shared_ptr<TokenComponentConst> component);
	void addComponentStatic(std::shared_ptr<TokenComponentStatic> component);
	void addComponentSignature(std::shared_ptr<TokenComponentSignature> component);

	// Logging.
	std::string getTypeString(NodeType type) const;
	std::string getAsString() const;

private:
	void operator=(const Node&);

	NodeType m_type;
	std::string m_name;

	std::vector<Edge*> m_edges;
};

std::ostream& operator<<(std::ostream& ostream, const Node& node);

#endif // NODE_H
