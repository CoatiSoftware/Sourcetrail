from enum import Enum
from random import randint


def numberIn():
	return int(input())


def numberOut(num):
	print(str(num), end="")


def stringOut(str):
	print(str, end="")


class GameObject:
	pass


class Field(GameObject):
	class Token(Enum):
		TOKEN_NONE = 0
		TOKEN_PLAYER_A = 1
		TOKEN_PLAYER_B = 4


	def opponent(self, token):
		if token == Field.Token.TOKEN_PLAYER_A:
			return Field.Token.TOKEN_PLAYER_B
		elif token == Field.Token.TOKEN_PLAYER_B:
			return Field.Token.TOKEN_PLAYER_A
		return Field.Token.TOKEN_NONE


	class Move:
		def __init__(self):
			self.row = None
			self.col = None


	def __init__(self):
		self._grid = []
		for j in range(0, 3):
			row = []
			for i in range(0, 3):
				row.append(Field.Token.TOKEN_NONE)
			self._grid.append(row)
		self._left = 9


	def clone(self):
		field = Field()
		for j in range(0, 3):
			for i in range(0, 3):
				field._grid[i][j] = self._grid[i][j]
		field._left = self._left
		return field


	def clear(self):
		for j in range(0, 3):
			for i in range(0, 3):
				self._grid[i][j] = Field.Token.TOKEN_NONE
		self._left = 9


	def show(self):
		stringOut("   1   2   3\n")
		for row in range(0, 3):
			numberOut(row + 1)
			stringOut(" ")
			for col in range(0, 3):
				if self._grid[row][col] == Field.Token.TOKEN_PLAYER_A:
					stringOut(" X ")
				elif self._grid[row][col] == Field.Token.TOKEN_PLAYER_B:
					stringOut(" O ")
				else:
					stringOut("   ")
				if col < 2:
					stringOut("|")
			if row < 2:
				stringOut("\n  -----------\n")
		stringOut("\n\n")


	def sameInRow(self, token, amount):
		total = amount * token.value
		count = 0
		for i in range(0, 3):
			if self._grid[i][0].value + self._grid[i][1].value + self._grid[i][2].value == sum:
				count += 1
			if self._grid[0][i].value + self._grid[1][i].value + self._grid[2][i].value == sum:
				count += 1
		if self._grid[0][0].value + self._grid[1][1].value + self._grid[2][2].value == sum:
			count += 1
		if self._grid[2][0].value + self._grid[1][1].value + self._grid[0][2].value == sum:
			count += 1
		return count


	def inRange(self, move):
		return move.row >= 0 and move.row < 3 and move.col >= 0 and move.col < 3


	def isEmpty(self, move):
		return self._grid[move.row][move.col] == Field.Token.TOKEN_NONE


	def isFull(self):
		return self._left == 0


	def makeMove(self, move, token):
		if not self.inRange(move):
			return
		if not self.isEmpty(move):
			return
		if token is Field.Token.TOKEN_NONE:
			return
		if self.isFull():
			return
		self._grid[move.row][move.col] = token
		self._left -= 1


	def clearMove(self, move):
		if not self.inRange(move):
			return
		if self.isEmpty(move):
			return
		if self._left == 9:
			return
		self._grid[move.row][move.col] = Field.Token.TOKEN_NONE
		self._left += 1


class Player(GameObject):
	def __init__(self, token, name):
		self.token = token
		self.name = name


	def turn(field):
		raise NotImplementedError


class HumanPlayer(Player):
	def __init__(self, token, name):
		Player.__init__(self, token, name)


	def turn(self, field):
		stringOut(self.name)
		stringOut("\n")
		while True:
			move = self._input()
			if self._check(field, move):
				break
		return move


	def _input(self):
		move = Field.Move()
		stringOut("Insert row: ")
		move.row = numberIn() - 1
		stringOut("Insert col: ")
		move.col = numberIn() - 1
		stringOut("\n")
		return move


	def _check(self, field, move):
		if not field.inRange(move):
			stringOut("Wrong input!\n")
			return False
		elif not field.isEmpty(move):
			stringOut("Is occupied!\n")
			return False
		return True


class ArtificialPlayer(Player):
	class Node:
		def __init__(self):
			self.move = None
			self.value = 0


	def __init__(self, token, name):
		Player.__init__(self, token, name)


	def turn(self, field):
		tempField = field.clone()
		node = self._minMax(tempField, self.token)
		return node.move


	def _minMax(self, field, token, prefix = ""):
		node = ArtificialPlayer.Node()
		node.value = -10000

		move = Field.Move()
		sameMove = 0

		for j in range(0, 3):
			move.row = j
			for i in range(0, 3):
				move.col = i

				if not field.isEmpty(move):
					continue

				field.makeMove(move, token)

				turnValue = self._evaluate(field, token)
				if turnValue == 0 and not field.isFull():
					turnValue = -self._minMax(field, field.opponent(token), prefix + " ").value

				field.clearMove(move)

				if turnValue > node.value:
					node.move = move
					node.value = turnValue
					sameMove = 1
				elif turnValue == node.value:
					sameMove += 1
					if randint(0, sameMove - 1) == 0:
						node.move = move
		return node


	def _evaluate(self, field, token):
		if field.sameInRow(token, 3):
			return 2
		elif field.sameInRow(field.opponent(token), 2):
			return -1
		elif field.sameInRow(token, 2) > 1:
			return 1
		return 0


class TicTacToe:
	def __init__(self):
		self._field = Field()
		self._players = [None, None]


	def start(self):
		self._reset()
		stringOut("Tic Tac Toe\n\n[1] Human\n[2] Computer\n[3] Quit\n\n")

		self._players[0] = self._selectPlayer(Field.Token.TOKEN_PLAYER_A, "Player A")
		if self._players[0] is None:
			return False

		self._players[1] = self._selectPlayer(Field.Token.TOKEN_PLAYER_B, "Player B")
		if self._players[1] is None:
			return False

		stringOut("\n")
		return True


	def run(self):
		self._field.show()
		playerIndex = 0
		for i in range(0, 9):
			player = self._players[playerIndex]
			self._field.makeMove(player.turn(self._field), player.token)
			self._field.show()
			if self._field.sameInRow(player.token, 3):
				stringOut(player.name)
				stringOut(" won!\n\n")
				return
			playerIndex = (playerIndex + 1) % 2
		stringOut("Game ends in draw!\n\n")


	def _reset(self):
		for i in range(0, 2):
			self._players[i] = None
		self._field.clear()


	def _selectPlayer(self, token, name):
		while (True):
			stringOut("Choose ")
			stringOut(name)
			stringOut(": ")
			selection = numberIn()
			if selection == 1:
				return HumanPlayer(token, name)
			elif selection == 2:
				return ArtificialPlayer(token, name)
			elif selection == 3:
				return None
			stringOut("Wrong input!\n")


def main():
	tictactoe = TicTacToe()
	while tictactoe.start():
		tictactoe.run()


if __name__ == '__main__':
	main()
