#ifndef NODE_H
#define NODE_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "data/graph/Edge.h"
#include "data/graph/Token.h"
#include "data/name/NameHierarchy.h"

class TokenComponentAbstraction;
class TokenComponentConst;
class TokenComponentStatic;
class TokenComponentFilePath;

class Node
	: public Token
{
public:
	typedef int NodeTypeMask;
	enum NodeType : NodeTypeMask
	{
		NODE_UNDEFINED					= 0x1,
		NODE_TYPE						= 0x2,

		NODE_NAMESPACE					= 0x4,
		NODE_STRUCT						= 0x8,
		NODE_CLASS						= 0x10,
		NODE_GLOBAL_VARIABLE			= 0x20,
		NODE_FIELD						= 0x40,
		NODE_FUNCTION					= 0x80,
		NODE_METHOD						= 0x100,

		NODE_ENUM						= 0x200,
		NODE_ENUM_CONSTANT				= 0x400,
		NODE_TYPEDEF					= 0x800,
		NODE_TEMPLATE_PARAMETER_TYPE	= 0x1000,

		NODE_FILE						= 0x2000,
		NODE_MACRO						= 0x4000
	};

	static std::string getTypeString(NodeType type);
	static int typeToInt(NodeType type);
	static NodeType intToType(int value);

	static const NodeTypeMask NODE_NOT_VISIBLE;

	Node(Id id, NodeType type, NameHierarchy nameHierarchy, bool defined);
	virtual ~Node();

	NodeType getType() const;
	void setType(NodeType type);
	bool isType(NodeTypeMask mask) const;

	std::string getName() const;
	std::string getFullName() const;
	NameHierarchy getNameHierarchy() const;

	bool isDefined() const;
	void setDefined(bool defined);

	const std::vector<Edge*>& getEdges() const;

	void addEdge(Edge* edge);
	void removeEdge(Edge* edge);

	Node* getParentNode() const;
	Node* getLastParentNode();
	Edge* getMemberEdge() const;

	Edge* findEdge(std::function<bool(Edge*)> func) const;
	Edge* findEdgeOfType(Edge::EdgeTypeMask mask) const;
	Edge* findEdgeOfType(Edge::EdgeTypeMask mask, std::function<bool(Edge*)> func) const;
	Node* findChildNode(std::function<bool(Node*)> func) const;

	void forEachEdge(std::function<void(Edge*)> func) const;
	void forEachEdgeOfType(Edge::EdgeTypeMask mask, std::function<void(Edge*)> func) const;
	void forEachChildNode(std::function<void(Node*)> func) const;

	bool hasReferences() const;

	// Token implementation.
	virtual bool isNode() const;
	virtual bool isEdge() const;

	// Component setters.
	void addComponentAbstraction(std::shared_ptr<TokenComponentAbstraction> component);
	void addComponentConst(std::shared_ptr<TokenComponentConst> component);
	void addComponentStatic(std::shared_ptr<TokenComponentStatic> component);
	void addComponentFilePath(std::shared_ptr<TokenComponentFilePath> component);

	// Logging.
	virtual std::string getTypeString() const;
	std::string getAsString() const;

private:
	void operator=(const Node&);

	std::vector<Edge*> m_edges;

	NodeType m_type;
	NameHierarchy m_nameHierarchy;
	bool m_defined;
};

std::ostream& operator<<(std::ostream& ostream, const Node& node);

#endif // NODE_H
