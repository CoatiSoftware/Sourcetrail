data.push( { type : 'file', name : 'human_player.h', content : [





	{ line : 7, type : 'class', name : 'HumanPlayer', base : 'Player', content : {
		public : [
			{ line : 9, type : 'method', name : 'HumanPlayer', return : 'HumanPlayer', params : [ 'Field::Token', 'std::string' ], content : [
				{ line : 10, type : 'call', class : 'Player', method : 'Player' }
			]},

			{ line : 13, type : 'method', name : 'Turn', abstraction : 'virtual', return : 'Field::Move', params : [ 'Field' ], content : [

				{ line : 15, type : 'use', class : 'std::cout' },

				{ line : 17, type : 'call', method : 'Input' },
				{ line : 18, type : 'use', class : 'Move', member : 'row' },
				{ line : 19, type : 'use', class : 'Move', member : 'col' },
				{ line : 20, type : 'call', method : 'Check' }

			]},

		], private : [
			{ line : 25, type : 'method', name : 'Input', return : 'Field::Move', content : [
				{ line : 26, type : 'use', class : 'Move' },

				{ line : 28, type : 'use', class : 'std::cout' },
				{ line : 29, type : 'call', function : 'input::number' }, { line : 29, type : 'use', class : 'Move', member : 'row' },

				{ line : 31, type : 'use', class : 'std::cout' },
				{ line : 32, type : 'call', function : 'input::number' }, { line : 32, type : 'use', class : 'Move', member : 'col' },

				{ line : 34, type : 'use', class : 'std::cout' }

			]},

			{ line : 38, type : 'method', name : 'Check', return : 'bool', params : [ 'Field', 'Field::Move' ], content : [
				{ line : 39, type : 'call', class : 'Field', method : 'InRange' },
				{ line : 40, type : 'use', class : 'std::cout' },

				{ line : 42, type : 'call', class : 'Field', method : 'IsEmpty' },
				{ line : 43, type : 'use', class : 'std::cout' }



			]}
		]}
	}
]});
