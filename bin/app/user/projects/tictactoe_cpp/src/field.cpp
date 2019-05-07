#include "field.h"

#include "io.h"

Field::Token Field::Opponent( Token token ) {
	if ( token == PlayerA ) {
		return PlayerB;
	} else if (token == PlayerB){
		return PlayerA;
	} else {
		return None;
	}
}

Field::Field()
	: left_( 9 ) {
	grid_ = new Token* [3];
	for ( int i = 0; i < 3 ; i++ ) {
		grid_[i] = new Token[3];
	}
}

Field::~Field() {
	for ( int i = 0; i < 3; i++ ) {
		delete [] grid_[i];
	}
	delete [] grid_;
}

Field Field::Clone() const {
	Field field;
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			field.grid_[i][j] = grid_[i][j];
		}
	}
	field.left_ = left_;
	return field;
}

void Field::Clear() {
	for ( int i = 0; i < 3; i++ ) {
		for ( int j = 0; j < 3; j++ ) {
			grid_[i][j] = None;
		}
	}
	left_ = 9;
}

void Field::Show() const {
	io::stringOut("   1   2   3\n");
	for ( int row = 0; row < 3; row++ ) {
		io::numberOut(row + 1);
		io::stringOut(" ");

		for ( int col = 0; col < 3; col++ ) {
			if ( grid_[row][col] == PlayerA ) {
				io::stringOut(" X ");
			} else if ( grid_[row][col] == PlayerB ) {
				io::stringOut(" O ");
			} else {
				io::stringOut("   ");
			}

			if ( col < 2 ) {
				io::stringOut("|");
			}
		}

		if ( row < 2 ) {
			io::stringOut("\n  -----------\n");
		}
	}
	io::stringOut("\n\n");
}

int Field::SameInRow( Token token, int amount ) const {
	int sum = amount * token;
	int count = 0;

	for ( int i = 0; i < 3; i++ ) {
		if ( grid_[i][0] + grid_[i][1] + grid_[i][2] == sum ) {
			count++;
		} 
		if ( grid_[0][i] + grid_[1][i] + grid_[2][i] == sum ) {
			count++;
		}
	}

	if ( grid_[0][0] + grid_[1][1] + grid_[2][2] == sum ) {
		count++;
	} 
	if ( grid_[2][0] + grid_[1][1] + grid_[0][2] == sum ) {
		count++;
	}

	return count;
}

bool Field::InRange( const Move& move ) const {
	return move.row >= 0 && move.row < 3 && move.col >= 0 && move.col < 3;
}

bool Field::IsEmpty( const Move& move ) const {
	return grid_[move.row][move.col] == None;
}

bool Field::IsFull() const {
	return left_ == 0;
}

void Field::MakeMove( const Move& move, Token token ) {
	if ( !InRange( move ) || !IsEmpty( move ) || token == None || !left_ ) {
		return;
	}

	grid_[move.row][move.col] = token;
	left_--;
}

void Field::ClearMove( const Move& move ) {
	if ( !InRange( move ) || IsEmpty( move ) || left_ == 9 ) {
		return;
	}

	grid_[move.row][move.col] = None;
	left_++;
}
