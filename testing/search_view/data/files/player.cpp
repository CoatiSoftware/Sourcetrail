#include "player.h"

Player::Player( Field::Token token, const char* name )
	: token_( token )
	, name_( name ) {
}

Player::~Player() {
}

const Field::Token& Player::getToken() const
{
	return token_;
}

const char* Player::getName() const
{
	return name_;
}
