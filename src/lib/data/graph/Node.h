#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <functional>
#include <map>
#include <string>

#include "DefinitionKind.h"
#include "Edge.h"
#include "NameHierarchy.h"
#include "NodeType.h"
#include "Token.h"

class Node: public Token
{
public:
	Node(Id id, NodeType type, NameHierarchy nameHierarchy, DefinitionKind definitionKind);
	Node(const Node& other);
	virtual ~Node();

	NodeType getType() const;
	void setType(NodeType type);
	bool isType(NodeKindMask mask) const;

	std::wstring getName() const;
	std::wstring getFullName() const;
	const NameHierarchy& getNameHierarchy() const;

	bool isDefined() const;
	bool isImplicit() const;
	bool isExplicit() const;

	size_t getChildCount() const;
	void setChildCount(size_t childCount);

	size_t getEdgeCount() const;

	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);

	Node* getParentNode() const;
	Node* getLastParentNode();
	Edge* getMemberEdge() const;
	bool isParentOf(const Node* node) const;

	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Edge* findEdgeOfType(Edge::TypeMask mask) const;
	Edge* findEdgeOfType(Edge::TypeMask mask, std::function<bool(Edge*)> func) const;
	Node* findChildNode(std::function<bool(Node*)> func) const;

	void forEachEdge(std::function<void(Edge*)> func) const;
	void forEachEdgeOfType(Edge::TypeMask mask, std::function<void(Edge*)> func) const;
	void forEachChildNode(std::function<void(Node*)> func) const;
	void forEachNodeRecursive(std::function<void(const Node*)> func) const;

	// Token implementation.
	virtual bool isNode() const override;
	virtual bool isEdge() const override;

	// Logging.
	virtual std::wstring getReadableTypeString() const override;
	std::wstring getAsString() const;

private:
	void operator=(const Node&);

	std::map<Id, Edge*> m_edges;

	NodeType m_type;
	const NameHierarchy m_nameHierarchy;
	DefinitionKind m_definitionKind;

	size_t m_childCount;
};

std::wostream& operator<<(std::wostream& ostream, const Node& node);

#endif	  // NODE_H
