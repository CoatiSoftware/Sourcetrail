#ifndef TOKEN_COMPONENT_AGGREGATION_H
#define TOKEN_COMPONENT_AGGREGATION_H

#include <set>

#include "utility/types.h"

#include "data/graph/token_component/TokenComponent.h"

class TokenComponentAggregation
	: public TokenComponent
{
public:
	TokenComponentAggregation();
	virtual ~TokenComponentAggregation();

	virtual std::shared_ptr<TokenComponent> copy() const;

	int getAggregationCount() const;
	const std::set<Id>& getAggregationIds() const;

	void addAggregationId(Id id);
	void removeAggregationId(Id id);

private:
	std::set<Id> m_ids;
};

#endif // TOKEN_COMPONENT_AGGREGATION_H
