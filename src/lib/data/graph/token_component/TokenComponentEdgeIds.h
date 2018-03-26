#ifndef TOKEN_COMPONENT_EDGE_IDS_H
#define TOKEN_COMPONENT_EDGE_IDS_H

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentEdgeIds
	: public TokenComponent
{
public:
	TokenComponentEdgeIds(const std::vector<Id>& edgeIds)
		: edgeIds(edgeIds)
	{
	}

	virtual std::shared_ptr<TokenComponent> copy() const
	{
		return std::make_shared<TokenComponentEdgeIds>(*this);
	}

	const std::vector<Id> edgeIds;
};

#endif // TOKEN_COMPONENT_EDGE_IDS_H
