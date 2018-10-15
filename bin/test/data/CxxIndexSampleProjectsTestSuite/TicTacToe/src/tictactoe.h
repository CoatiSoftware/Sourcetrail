#ifndef _TIC_TAC_TOE_
#define _TIC_TAC_TOE_

#include "field.h"
#include "player.h"

class TicTacToe {
public:
	TicTacToe();
	~TicTacToe();

	bool Start();
	void Run();

private:
	void Reset();
	Player* SelectPlayer( Field::Token token, const char* name ) const;

	Player* players_[2];
	Field field_;
};

#endif // _TIC_TAC_TOE_
