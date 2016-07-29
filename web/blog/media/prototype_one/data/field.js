data.push( { type : 'file', name : 'field.h', content : [





	{ line : 7, type : 'class', name : 'Field', content : {
		public : [
			{ line : 9, type : 'class', name : 'Token', content : { public : [
					{ line : 10, type : 'member', name : 'None' },
					{ line : 11, type : 'member', name : 'PlayerA' },
					{ line : 12, type : 'member', name : 'PlayerB' }
			]}},

			{ line : 15, type : 'method', name : 'Opponent', abstraction : 'static', return : 'Token', params : [ 'Token' ], content : [
				{ line : 16, type : 'call', function : 'assert' }, { line : 16, type : 'use', class : 'Token', member : 'None' },
				{ line : 17, type : 'use', class : 'Token', member : 'PlayerA' },
				{ line : 18, type : 'use', class : 'Token', member : 'PlayerB' },

				{ line : 20, type : 'use', class : 'Token', member : 'PlayerA' }

			]},

			{ line : 24, type : 'class', name : 'Move', content : { public : [
					{ line : 25, type : 'member', name : 'row', class : 'int' },
					{ line : 26, type : 'member', name : 'col', class : 'int' }
			]}},

			{ line : 29, type : 'method', name : 'Field', content : [
				{ line : 30, type : 'use', member : 'left_' },
				{ line : 31, type : 'use', member : 'grid_' }, { line : 31, type : 'use', class : 'Token' },

				{ line : 33, type : 'use', member : 'grid_' }, { line : 33, type : 'use', class : 'Token' },

			]},

			{ line : 37, type : 'method', name : '~Field', content : [

				{ line : 39, type : 'use', member : 'grid_' },

				{ line : 41, type : 'use', member : 'grid_' },
			]},

			{ line : 44, type : 'method', name : 'Clone', return : 'Field', content : [



				{ line : 48, type : 'use', member : 'grid_' }, { line : 48, type : 'use', member : 'grid_' },


				{ line : 51, type : 'use', member : 'left_' }, { line : 51, type : 'use', member : 'left_' }
			]},


			{ line : 55, type : 'method', name : 'Clear', content : [


				{ line : 58, type : 'use', member : 'grid_' }, { line : 58, type : 'use', class : 'Token', member : 'None' },


				{ line : 61, type : 'use', member : 'left_' }
			]},

			{ line : 64, type : 'method', name : 'Show', content : [
				{ line : 65, type : 'use', class : 'std::cout' },

				{ line : 67, type : 'use', class : 'std::cout' },


				{ line : 70, type : 'use', member : 'grid_' }, { line : 70, type : 'use', class : 'Token', member : 'PlayerA' },
				{ line : 71, type : 'use', class : 'std::cout' },
				{ line : 72, type : 'use', member : 'grid_' }, { line : 71, type : 'use', class : 'Token', member : 'PlayerA' },
				{ line : 73, type : 'use', class : 'std::cout' },

				{ line : 75, type : 'use', class : 'std::cout' },



				{ line : 79, type : 'use', class : 'std::cout' },




				{ line : 84, type : 'use', class : 'std::cout' },


				{ line : 87, type : 'use', class : 'std::cout' }
			]},

			{ line : 90, type : 'method', name : 'SameInRow', return : 'int', params : [ 'Token', 'int' ], content : [




				{ line : 95, type : 'use', member : 'grid_' }, { line : 95, type : 'use', member : 'grid_' }, { line : 95, type : 'use', member : 'grid_' },

				{ line : 97, type : 'use', member : 'grid_' }, { line : 97, type : 'use', member : 'grid_' }, { line : 97, type : 'use', member : 'grid_' },




				{ line : 102, type : 'use', member : 'grid_' }, { line : 102, type : 'use', member : 'grid_' }, { line : 102, type : 'use', member : 'grid_' },

				{ line : 104, type : 'use', member : 'grid_' }, { line : 104, type : 'use', member : 'grid_' }, { line : 104, type : 'use', member : 'grid_' },




			]},

			{ line : 111, type : 'method', name : 'InRange', return : 'bool', params : [ 'Move' ], content : [
				{ line : 112, type : 'use', class : 'Move', member : 'row' }, { line : 112, type : 'use', class : 'Move', member : 'row' }, { line : 112, type : 'use', class : 'Move', member : 'col' }, { line : 112, type : 'use', class : 'Move', member : 'col' }
			]},

			{ line : 115, type : 'method', name : 'IsEmpty', return : 'bool', params : [ 'Move' ], content : [
				{ line : 116, type : 'use', member : 'grid_' }, { line : 116, type : 'use', class : 'Move', member : 'row' }, { line : 116, type : 'use', class : 'Move', member : 'col' }, { line : 116, type : 'use', class : 'Token', member : 'None' }
			]},

			{ line : 119, type : 'method', name : 'IsFull', return : 'bool', content : [
				{ line : 120, type : 'use', member : 'left_' }
			]},

			{ line : 123, type : 'method', name : 'MakeMove', params : [ 'Move', 'Token' ], content : [
				{ line : 124, type : 'call', function : 'assert' }, { line : 124, type : 'call', method : 'InRange' },
				{ line : 125, type : 'call', function : 'assert' }, { line : 125, type : 'call', method : 'IsEmpty' },
				{ line : 126, type : 'call', function : 'assert' }, { line : 126, type : 'use', class : 'Token', member : 'None' },
				{ line : 127, type : 'call', function : 'assert' }, { line : 127, type : 'use', member : 'left_' },

				{ line : 129, type : 'use', member : 'grid_' }, { line : 129, type : 'use', class : 'Move', member : 'row' }, { line : 129, type : 'use', class : 'Move', member : 'row' },
				{ line : 130, type : 'use', member : 'left_' },
			]},

			{ line : 133, type : 'method', name : 'ClearMove', params : [ 'Move' ], content : [
				{ line : 134, type : 'call', function : 'assert' }, { line : 134, type : 'call', method : 'InRange' },
				{ line : 135, type : 'call', function : 'assert' }, { line : 135, type : 'call', method : 'IsEmpty' },
				{ line : 136, type : 'call', function : 'assert' }, { line : 136, type : 'use', member : 'left_' },

				{ line : 138, type : 'use', member : 'grid_' }, { line : 138, type : 'use', class : 'Move', member : 'row' }, { line : 138, type : 'use', class : 'Move', member : 'row' }, { line : 138, type : 'use', class : 'Token', member : 'None' },
				{ line : 139, type : 'use', member : 'left_' },
			]}

		], private : [
			{ line : 143, type : 'member', name : 'grid_', class : 'Token' },
			{ line : 144, type : 'member', name : 'left_', class : 'int' }
		]}
	}
]});