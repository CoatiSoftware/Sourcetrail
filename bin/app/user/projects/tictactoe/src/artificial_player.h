#ifndef _ARTIFICIAL_PLAYER_
#define _ARTIFICIAL_PLAYER_

#include "player.h"

class ArtificialPlayer : public Player {
public:
	ArtificialPlayer( Field::Token token, const char* name  );
	virtual ~ArtificialPlayer();

	virtual Field::Move Turn( const Field& field ) const;

private:
	struct Node {
		Field::Move move;
		int value;
	};

	Node MinMax( Field& field, Field::Token token ) const;

	int Evaluate( const Field& field, Field::Token token ) const;
};

#endif // _ARTIFICIAL_PLAYER_
