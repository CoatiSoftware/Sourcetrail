#ifndef _HUMAN_
#define _HUMAN_

#include "input.h"
#include "player.h"

class HumanPlayer : public Player {
public:
	HumanPlayer( Field::Token token, const std::string& name  )
		: Player( token, name ) {
	}
	virtual ~HumanPlayer() {}

	virtual Field::Move Turn( const Field& field ) const {
		Field::Move move;
		std::cout << name_ << '\n';
		do {
			move = Input();
			move.row -= 1;
			move.col -= 1;
		} while ( !Check( field, move ) );
		return move;
	}

private:
	Field::Move Input() const {
		Field::Move move;

		std::cout << "Insert row: ";
		input::number( &move.row );

		std::cout << "Insert column: ";
		input::number( &move.col );

		std::cout << '\n';
		return move;
	}

	bool Check( const Field& field, const Field::Move& move ) const {
		if ( !field.InRange( move ) ) {
			std::cout << "Wrong input!\n";
			return false;
		} else if ( !field.IsEmpty( move ) ) {
			std::cout << "Is occupied!\n";
			return false;
		}
		return true;
	}
};

#endif // _HUMAN_
