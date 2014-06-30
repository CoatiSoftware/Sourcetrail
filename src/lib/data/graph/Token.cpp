#include "data/graph/Token.h"

Token::Token()
	: m_id(s_nextId++)
{
}

Token::~Token()
{
}

Id Token::getId() const
{
	return m_id;
}

Id Token::s_nextId = 1;

Token::Token(Id id)
	: m_id(id)
{
}
