#ifndef _FIELD_
#define _FIELD_

#include <cassert>
#include <iostream>

class Field {
public:
	enum Token {
		None = 0,
		PlayerA = 1,
		PlayerB = 4
	};

	static Token Opponent( Token token ) {
		assert( token != None );
		if ( token == PlayerA ) {
			return PlayerB;
		} else {
			return PlayerA;
		}
	}

	struct Move {
		int row;
		int col;
	};

	Field()
		: left_( 9 ) {
		grid_ = new Token* [3];
		for ( int i = 0; i < 3 ; i++ ) {
			grid_[i] = new Token[3];
		}
	}

	~Field() {
		for ( int i = 0; i < 3; i++ ) {
			delete grid_[i];
		}
		delete [] grid_;
	}

	Field Clone() const {
		Field field;
		for ( int i = 0; i < 3; i++ ) {
			for ( int j = 0; j < 3; j++ ) {
				field.grid_[i][j] = grid_[i][j];
			}
		}
		field.left_ = left_;
		return field;
	}

	void Clear() {
		for ( int i = 0; i < 3; i++ ) {
			for ( int j = 0; j < 3; j++ ) {
				grid_[i][j] = None;
			}
		}
		left_ = 9;
	}

	void Show() const {
		std::cout << "   1   2   3\n";
		for ( int row = 0; row < 3; row++ ) {
			std::cout << row + 1 << " ";

			for ( int col = 0; col < 3; col++ ) {
				if ( grid_[row][col] == PlayerA ) {
					std::cout << " X ";
				} else if ( grid_[row][col] == PlayerB ) {
					std::cout << " O ";
				} else {
					std::cout << "   ";
				}

				if ( col < 2 ) {
					std::cout << "|";
				}
			}

			if ( row < 2 ) {
				std::cout << "\n  -----------\n";
			}
		}
		std::cout << "\n\n";
	}

	int SameInRow( Token token, int amount ) const {
		int sum = amount * token;
		int count = 0;

		for ( int i = 0; i < 3; i++ ) {
			if ( grid_[i][0] + grid_[i][1] + grid_[i][2] == sum ) {
				count++;
			} else if ( grid_[0][i] + grid_[1][i] + grid_[2][i] == sum ) {
				count++;
			}
		}

		if ( grid_[0][0] + grid_[1][1] + grid_[2][2] == sum ) {
			count++;
		} else if ( grid_[2][0] + grid_[1][1] + grid_[0][2] == sum ) {
			count++;
		}

		return count;
	}

	bool InRange( const Move& move ) const {
		return move.row >= 0 && move.row < 3 && move.col >= 0 && move.col < 3;
	}

	bool IsEmpty( const Move& move ) const {
		return grid_[move.row][move.col] == None;
	}

	bool IsFull() const {
		return left_ == 0;
	}

	void MakeMove( const Move& move, Token token ) {
		assert( InRange( move ) );
		assert( IsEmpty( move ) );
		assert( token != None );
		assert( left_ );

		grid_[move.row][move.col] = token;
		left_--;
	}

	void ClearMove( const Move& move ) {
		assert( InRange(move) );
		assert( !IsEmpty(move) );
		assert( left_ < 9 );

		grid_[move.row][move.col] = None;
		left_++;
	}

private:
	Token** grid_;
	int left_;
};

#endif // _FIELD_
