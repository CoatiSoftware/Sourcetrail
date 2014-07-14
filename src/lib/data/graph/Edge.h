#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <string>

#include "data/graph/Token.h"

class Node;
class EdgeComponent;

class Edge: public Token
{
public:
	enum EdgeType
	{
		EDGE_MEMBER,
		EDGE_TYPE_OF,
		EDGE_RETURN_TYPE_OF,
		EDGE_PARAMETER_TYPE_OF,
		EDGE_USAGE,
		EDGE_CALL,
		EDGE_INHERITANCE,
		EDGE_TYPEDEF_OF
	};

	enum AccessType
	{
		ACCESS_PUBLIC,
		ACCESS_PROTECTED,
		ACCESS_PRIVATE,
		ACCESS_NONE
	};

	Edge(EdgeType type, Node* from, Node* to);
	virtual ~Edge();

	std::shared_ptr<Edge> createPlainCopy(Node* from, Node* to) const;

	EdgeType getType() const;

	Node* getFrom() const;
	Node* getTo() const;

	// Token implementation
	virtual bool isNode() const;
	virtual bool isEdge() const;

	// Additional field accessors for different EdgeTypes.
	AccessType getAccess() const;
	void setAccess(AccessType access);

	// Logging.
	std::string getTypeString() const;
	std::string getAccessString() const;
	std::string getAsString() const;


	void addComponent(std::shared_ptr<EdgeComponent> component);

	template <typename ComponentType>
	std::shared_ptr<ComponentType> getComponent() const;

	template <typename ComponentType>
	bool hasComponent() const;

private:
	// Constructor for plain copies.
	Edge(Id id, EdgeType type, Node* from, Node* to);

	const EdgeType m_type;

	Node* const m_from;
	Node* const m_to;

	// Additional fields for different EdgeTypes.
	AccessType m_access;

	std::vector<std::shared_ptr<EdgeComponent>> m_components;
};

template <typename ComponentType>
std::shared_ptr<ComponentType> Edge::getComponent() const
{
	std::shared_ptr<ComponentType> component;
	for (std::shared_ptr<EdgeComponent> c: m_components)
	{
		component = std::dynamic_pointer_cast<ComponentType>(c);
		if (component)
			break;
	}
	return component;
}

template <typename ComponentType>
bool Edge::hasComponent() const
{
	return (getComponent<ComponentType>());
}

std::ostream& operator<<(std::ostream& ostream, const Edge& edge);

#endif // EDGE_H
