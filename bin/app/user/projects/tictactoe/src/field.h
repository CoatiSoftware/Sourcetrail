#ifndef _FIELD_
#define _FIELD_

#include "game_object.h"

class Field : public GameObject {
public:
	enum Token {
		None = 0,
		PlayerA = 1,
		PlayerB = 4
	};

	static Token Opponent( Token token );

	struct Move {
		int row;
		int col;
	};

	Field();
	~Field();

	Field Clone() const;
	void Clear();

	void Show() const;

	int SameInRow( Token token, int amount ) const;

	bool InRange( const Move& move ) const;
	bool IsEmpty( const Move& move ) const;
	bool IsFull() const;

	void MakeMove( const Move& move, Token token );
	void ClearMove( const Move& move );

private:
	Token** grid_;
	int left_;
};

#endif // _FIELD_
