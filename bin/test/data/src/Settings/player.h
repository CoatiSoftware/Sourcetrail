#ifndef _PLAYER_
#define _PLAYER_

#include <string>

#include "field.h"

class Player {
public:
	Player( Field::Token token, const std::string& name )
		: token_( token )
		, name_( name ) {
	}

	virtual Field::Move Turn( const Field& field ) const = 0;

	const Field::Token token_;
	const std::string name_;
};

#endif // _PLAYER_
