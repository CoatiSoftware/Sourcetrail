#include "human_player.h"

#include "io.h"

HumanPlayer::HumanPlayer( Field::Token token, const char* name  )
	: Player( token, name ) {
}

HumanPlayer::~HumanPlayer() {
}

Field::Move HumanPlayer::Turn( const Field& field ) const {
	Field::Move move;
	io::stringOut(name_);
	io::stringOut("\n");

	do {
		move = Input();
		move.row -= 1;
		move.col -= 1;
	} while ( !Check( field, move ) );
	return move;
}

Field::Move HumanPlayer::Input() const {
	Field::Move move;
	move.row = -1;
	move.col = -1;

	io::stringOut("Insert row: ");
	move.row = io::numberIn();

	io::stringOut("Insert column: ");
	move.col = io::numberIn();

	io::stringOut("\n");
	return move;
}

bool HumanPlayer::Check( const Field& field, const Field::Move& move ) const {
	if ( !field.InRange( move ) ) {
		io::stringOut("Wrong input!\n");
		return false;
	} else if ( !field.IsEmpty( move ) ) {
		io::stringOut("Is occupied!\n");
		return false;
	}
	return true;
}
