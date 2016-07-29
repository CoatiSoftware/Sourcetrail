data.push( { type : 'file', name : 'computer.h', content : [




	{ line : 6, type : 'class', name : 'ArtificialPlayer', base : 'Player', content : {
		public : [
			{ line : 8, type : 'method', name : 'ArtificialPlayer', params : [ 'Field::Token', 'std::string' ], content : [
				{ line : 9, type : 'call', class : 'Player', method : 'Player' }
			]},

			{ line : 12, type : 'method', name : 'Turn', abstraction : 'virtual', return : 'Field::Move', params : [ 'Field' ], content : [
				{ line : 13, type : 'call', function : 'srand' }, { line : 13, type : 'call', function : 'time' },
				{ line : 14, type : 'call', class : 'Field', method : 'Clone' },
				{ line : 15, type : 'use', class : 'ArtificialPlayer::Node' }, { line : 15, type : 'call', method : 'MinMax' }, { line : 15, type : 'use', member : 'token_' },
				{ line : 16, type : 'use', class : 'ArtificialPlayer::Node', member : 'move' }
			]},

		], private : [
			{ line : 20, type : 'class', name : 'Node', content : { public : [
					{ line : 21, type : 'member', name : 'move', class : 'Field::Move' },
					{ line : 22, type : 'member', name : 'value', class : 'int' }
				]}
			},
			{ line : 25, type : 'method', name : 'MinMax', return : 'Node', params : [ 'Field', 'Field::Token' ], content : [
				{ line : 26, type : 'use', class : 'Node' },


				{ line : 29, type : 'use', class : 'Move' },



				{ line : 33, type : 'use', class : 'Move', member : 'row' },


				{ line : 36, type : 'use', class : 'Move', member : 'col' },

				{ line : 38, type : 'call', class : 'Field', method : 'IsEmpty' },



				{ line : 42, type : 'call', class : 'Field', method : 'MakeMove' },

				{ line : 44, type : 'call', method : 'Evaluate' },
				{ line : 45, type : 'call', class : 'Field', method : 'IsFull' },
				{ line : 46, type : 'call', method : 'MinMax' }, { line : 46, type : 'call', class : 'Field', method : 'Opponent' }, { line : 46, type : 'use', class : 'Node', member : 'value' },


				{ line : 49, type : 'call', class : 'Field', method : 'ClearMove' },

				{ line : 51, type : 'use', class : 'Node', member : 'value' },
				{ line : 52, type : 'use', class : 'Node', member : 'move' },
				{ line : 53, type : 'use', class : 'Node', member : 'value' },
				{ line : 54, type : 'use', class : 'Node', member : 'value' },


				{ line : 57, type : 'call', function : 'rand' },
				{ line : 58, type : 'use', class : 'Node', member : 'move' },






			]},

			{ line : 67, type : 'method', name : 'Evaluate', return : 'int', params : [ 'Field', 'Field::Token' ], content : [
				{ line : 68, type : 'call', class : 'Field', method : 'SameInRow' },

				{ line : 70, type : 'call', class : 'Field', method : 'SameInRow' }, { line : 70, type : 'call', class : 'Field', method : 'Opponent' },

				{ line : 72, type : 'call', class : 'Field', method : 'SameInRow' }



			]}
		]}
	}
]});
