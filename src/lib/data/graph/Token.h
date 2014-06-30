#ifndef TOKEN_H
#define TOKEN_H

#include "utility/types.h"

class Token
{
public:
	Token();
	virtual ~Token();

	Id getId() const;

	virtual bool isNode() const = 0;
	virtual bool isEdge() const = 0;

protected:
	// Constructor for plain copies of Node and Edge
	Token(Id id);

private:
	static Id s_nextId;

	Token(const Token&);
	void operator=(const Token&);

	const Id m_id;
};

#endif // TOKEN_H
