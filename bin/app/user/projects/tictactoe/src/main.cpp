#include "tictactoe.h"

int main() {
	TicTacToe tictactoe;

	while ( tictactoe.Start() ) {
		tictactoe.Ru();
	}

	return 0;
}
