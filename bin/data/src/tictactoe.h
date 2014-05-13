#ifndef _TIC_TAC_TOE_
#define _TIC_TAC_TOE_

#include "computer.h"
#include "field.h"
#include "human.h"
#include "input.h"
#include "player.h"

class TicTacToe {
public:
	TicTacToe() {
		players_[0] = NULL;
		players_[1] = NULL;
	}

	~TicTacToe() {
		Reset();
	}

	bool Start() {
		Reset();
		std::cout << "Tic Tac Toe\n\n[1] Human\n[2] Computer\n[3] Quit\n\n";

		players_[0] = SelectPlayer( Field::PlayerA, "PlayerA" );
		if ( !players_[0] ) {
			return false;
		}

		players_[1] = SelectPlayer( Field::PlayerB, "PlayerB" );
		if ( !players_[1] ) {
			return false;
		}

		std::cout << '\n';
		return true;
	}

	void Run() {
		field_.Show();

		int playerIndex = 0;

		for ( int i = 0; i < 9; i++ ) {
			Player& player = *players_[playerIndex];

			field_.MakeMove( player.Turn( field_ ), player.token_ );
			field_.Show();

			if ( field_.SameInRow( player.token_, 3 ) ) {
				std::cout << player.name_ << " won!\n\n";
				return;
			}

			playerIndex = ( playerIndex + 1 ) % 2;
		}

		std::cout << "Game ends in draw!\n\n";
	}

private:
	void Reset() {
		field_.Clear();

		for ( int i = 0; i < 2; i++ ) {
			if ( players_[i] ) {
				delete players_[i];
				players_[i] = NULL;
			}
		}
	}

	Player* SelectPlayer( Field::Token token, const std::string& name ) const {
		int selection;

		while ( true ) {
			std::cout << "Choose " << name << ": ";
			input::number( &selection );

			switch ( selection ) {
				case 1 : return new HumanPlayer( token, name );
				case 2 : return new ArtificialPlayer( token, name );
				case 3 : return NULL;
				default : std::cout << "Wrong input!\n";
			}
		}
	}

	Player* players_[2];
	Field field_;
};

#endif // _TIC_TAC_TOE_