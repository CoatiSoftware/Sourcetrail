#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "data/graph/Edge.h"
#include "data/graph/Token.h"

class TokenComponentAbstraction;
class TokenComponentConst;
class TokenComponentName;
class TokenComponentStatic;
class TokenComponentSignature;

class Node: public Token
{
public:
	typedef int NodeTypeMask;
	enum NodeType : NodeTypeMask
	{
		NODE_UNDEFINED          = 0x1,
		NODE_UNDEFINED_FUNCTION = 0x2,
		NODE_UNDEFINED_VARIABLE = 0x4,
		NODE_UNDEFINED_TYPE     = 0x8,
		NODE_STRUCT             = 0x10,
		NODE_CLASS              = 0x20,
		NODE_GLOBAL_VARIABLE    = 0x40,
		NODE_FIELD              = 0x80,
		NODE_FUNCTION           = 0x100,
		NODE_METHOD             = 0x200,
		NODE_NAMESPACE          = 0x400,
		NODE_ENUM               = 0x800,
		NODE_TYPEDEF            = 0x1000
	};

	Node(NodeType type, const std::string& name);
	Node(NodeType type, std::shared_ptr<TokenComponentName> nameComponent);
	Node(const Node& other);
	virtual ~Node();

	NodeType getType() const;
	void setType(NodeType type);
	bool isType(NodeTypeMask mask) const;

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
	void addComponentAbstraction(std::shared_ptr<TokenComponentAbstraction> component);
	void addComponentConst(std::shared_ptr<TokenComponentConst> component);
	void addComponentStatic(std::shared_ptr<TokenComponentStatic> component);
	void addComponentSignature(std::shared_ptr<TokenComponentSignature> component);

	// Logging.
	std::string getTypeString(NodeType type) const;
	std::string getTypeString() const;
	std::string getAsString() const;

private:
	void operator=(const Node&);

	std::vector<Edge*> m_edges;

	NodeType m_type;
	std::shared_ptr<TokenComponentName> m_nameComponent;
};

std::ostream& operator<<(std::ostream& ostream, const Node& node);

#endif // NODE_H
