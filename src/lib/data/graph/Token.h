#ifndef TOKEN_H
#define TOKEN_H

#include <vector>

#include "utility/types.h"

class Token
{
public:
	Token();
	virtual ~Token();

	Id getId() const;

	virtual bool isNode() const = 0;
	virtual bool isEdge() const = 0;

	const std::vector<Id>& getLocationIds() const;

	void addLocationId(Id locationId);
	void removeLocationId(Id locationId);

protected:
	// Constructor for plain copies of Node and Edge
	Token(Id id);

private:
	static Id s_nextId;

	Token(const Token&);
	void operator=(const Token&);

	const Id m_id;	// own id

	std::vector<Id> m_locationIds;
};

#endif // TOKEN_H
