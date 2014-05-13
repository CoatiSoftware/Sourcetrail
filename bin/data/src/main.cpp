#include "tictactoe.h"

int main() {
	TicTacToe tictactoe;

	while ( tictactoe.Start() ) {
		tictactoe.Run();
	}

	return 0;
}