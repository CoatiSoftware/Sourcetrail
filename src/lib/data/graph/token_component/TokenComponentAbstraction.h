#ifndef TOKEN_COMPONENT_ABSTRACTION_H
#define TOKEN_COMPONENT_ABSTRACTION_H

#include <string>

#include "TokenComponent.h"

class TokenComponentAbstraction
	: public TokenComponent
{
public:
	enum AbstractionType
	{
		ABSTRACTION_VIRTUAL,
		ABSTRACTION_PURE_VIRTUAL,
		ABSTRACTION_NONE
	};

	TokenComponentAbstraction(AbstractionType abstraction);
	virtual ~TokenComponentAbstraction();

	virtual std::shared_ptr<TokenComponent> copy() const;

	AbstractionType getAbstraction() const;
	std::string getAbstractionString() const;

private:
	const AbstractionType m_abstraction;
};

#endif // TOKEN_COMPONENT_ABSTRACTION_H
