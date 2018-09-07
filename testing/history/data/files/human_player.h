#ifndef _HUMAN_PLAYER_
#define _HUMAN_PLAYER_

#include "player.h"


class HumanPlayer : public Player {
public:
	HumanPlayer( Field::Token token, const char* name  );
	virtual ~HumanPlayer();

	virtual Field::Move Turn( const Field& field ) const;

private:
	Field::Move Input() const;

	bool Check( const Field& field, const Field::Move& move ) const;
};

#endif // _HUMAN_PLAYER_
