#include "tictactoe.h"

#include "artificial_player.h"
#include "human_player.h"
#include "io.h"

TicTacToe::TicTacToe() {
	players_[0] = 0;
	players_[1] = 0;
}

TicTacToe::~TicTacToe() {
	Reset();
}

bool TicTacToe::Start() {
	Reset();
	io::stringOut("Tic Tac Toe\n\n[1] Human\n[2] Computer\n[3] Quit\n\n");

	players_[0] = SelectPlayer( Field::PlayerA, "Player A" );
	if ( !players_[0] ) {
		return false;
	}

	players_[1] = SelectPlayer( Field::PlayerB, "Player B" );
	if ( !players_[1] ) {
		return false;
	}

	io::stringOut("\n");
	return true;
}

void TicTacToe::Run() {
	field_.Show();

	int playerIndex = 0;

	for ( int i = 0; i < 9; i++ ) {
		Player& player = *players_[playerIndex];

		field_.MakeMove( player.Turn( field_ ), player.getToken() );
		field_.Show();

		if ( field_.SameInRow( player.getToken(), 3 ) ) {
			io::stringOut(player.getName());
			io::stringOut(" won!\n\n");
			return;
		}

		playerIndex = ( playerIndex + 1 ) % 2;
	}

	io::stringOut("Game ends in draw!\n\n");
}

void TicTacToe::Reset() {
	field_.Clear();

	for ( int i = 0; i < 2; i++ ) {
		if ( players_[i] ) {
			delete players_[i];
			players_[i] = 0;
		}
	}
}

Player* TicTacToe::SelectPlayer( Field::Token token, const char* name ) const {
	int selection = 0;

	while ( true ) {
		io::stringOut("Choose ");
		io::stringOut(name);
		io::stringOut(": ");

		selection = io::numberIn();

		switch ( selection ) {
			case 1 : return new HumanPlayer( token, name );
			case 2 : return new ArtificialPlayer( token, name );
			case 3 : return 0;
			default : io::stringOut("Wrong input!\n");
		}
	}
}
