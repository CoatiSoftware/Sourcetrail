data.push( { type : 'file', name : 'tictactoe.h', content : [








	{ line : 10, type : 'class', name : 'TicTacToe', content : {
		public : [
			{ line : 12, type : 'method', name : 'TicTacToe', content : [
				{ line : 13, type : 'use', member : 'players_' },
				{ line : 14, type : 'use', member : 'players_' }
			]},

			{ line : 17, type : 'method', name : '~TicTacToe', content : [
				{ line : 18, type : 'call', method : 'Reset' }
			]},

			{ line : 21, type : 'method', name : 'Start', return : 'bool', content : [
				{ line : 22, type : 'call', method : 'Reset' },
				{ line : 23, type : 'use', class : 'std::cout' },

				{ line : 25, type : 'use', member : 'players_' }, { line : 25, type : 'call', method : 'SelectPlayer' }, { line : 25, type : 'use', class : 'Field::Token', member : 'PlayerA' },
				{ line : 26, type : 'use', member : 'players_' },



				{ line : 30, type : 'use', member : 'players_' }, { line : 30, type : 'call', method : 'SelectPlayer' }, { line : 30, type : 'use', class : 'Field::Token', member : 'PlayerB' },
				{ line : 31, type : 'use', member : 'players_' },



				{ line : 35, type : 'use', class : 'std::cout' },

			]},

			{ line : 39, type : 'method', name : 'Run', content : [
				{ line : 40, type : 'use', member : 'field_' }, { line : 40, type : 'call', method : 'Show', class : 'Field' },




				{ line : 45, type : 'use', member : 'players_' },

				{ line : 47, type : 'use', member : 'field_' }, { line : 47, type : 'call', method : 'MakeMove', class : 'Field' }, { line : 47, type : 'call', method : 'Turn', class : 'Player' }, { line : 47, type : 'use', member : 'token_', class : 'Player' },
				{ line : 48, type : 'use', member : 'field_' }, { line : 48, type : 'call', method : 'Show', class : 'Field' },

				{ line : 50, type : 'use', member : 'field_' }, { line : 50, type : 'call', method : 'SameInRow', class : 'Field' }, { line : 50, type : 'use', member : 'token_', class : 'Player' },
				{ line : 51, type : 'use', class : 'std::cout' }, { line : 51, type : 'use', member : 'name_', class : 'Player' },






				{ line : 58, type : 'use', class : 'std::cout' }
			]},

		], private : [
			{ line : 62, type : 'method', name : 'Reset', content : [
				{ line : 63, type : 'use', member : 'field_' }, { line : 63, type : 'call', method : 'Clear', class : 'Field' },


				{ line : 66, type : 'use', member : 'players_' },
				{ line : 67, type : 'use', member : 'players_' },
				{ line : 68, type : 'use', member : 'players_' }


			]},

			{ line : 73, type : 'method', name : 'SelectPlayer', return : 'Player', params : [ 'Field::Token', 'std::string' ], content : [



				{ line : 77, type : 'use', class : 'std::cout' },
				{ line : 78, type : 'call', function : 'input::number' },


				{ line : 81, type : 'call', method : 'HumanPlayer', class : 'HumanPlayer' },
				{ line : 82, type : 'call', method : 'ArtificialPlayer', class : 'ArtificialPlayer' },

				{ line : 84, type : 'use', class : 'std::cout' }


			]},

			{ line : 89, type : 'member', name : 'players_', class : 'Player' },
			{ line : 90, type : 'member', name : 'field_', class : 'Field' }
		]}
	}
]});