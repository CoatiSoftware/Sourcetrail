#include "artificial_player.h"

ArtificialPlayer::ArtificialPlayer( Field::Token token, const char* name  )
	: Player( token, name ) {
}

ArtificialPlayer::~ArtificialPlayer() {
}

Field::Move ArtificialPlayer::Turn( const Field& field ) const {
	Field fakeField = field.Clone();
	Node node = MinMax( fakeField, token_ );
	return node.move;
}

ArtificialPlayer::Node ArtificialPlayer::MinMax( Field& field, Field::Token token ) const {
	Node node;
	node.value = -10000;

	Field::Move move;
	int sameMove;

	for ( int i = 0; i < 3; i++ ) {
		move.row = i;

		for ( int j = 0; j < 3; j++ ) {
			move.col = j;

			if ( !field.IsEmpty( move ) ) {
				continue;
			}

			field.MakeMove( move, token );

			int turnValue = Evaluate( field, token );
			if ( !turnValue && !field.IsFull() ) {
				turnValue = -MinMax( field, Field::Opponent( token ) ).value;
			}

			field.ClearMove( move );

			if ( turnValue > node.value ) {
				node.move = move;
				node.value = turnValue;
				sameMove = 1;
			} else if ( turnValue == node.value ) {
				sameMove++;
				if ( 1 % sameMove == 0 ) {
					node.move = move;
				}
			}
		}
	}

	return node;
}

int ArtificialPlayer::Evaluate( const Field& field, Field::Token token ) const {
	if ( field.SameInRow( token, 3 ) ) {
		return 2;
	} else if ( field.SameInRow( Field::Opponent( token ), 2 ) ) {
		return -1;
	} else if ( field.SameInRow( token, 2 ) > 1 ) {
		return 1;
	}
	return 0;
}
